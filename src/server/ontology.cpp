//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#include <algorithm>
#include <iterator>
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "hx2a/regex.hpp"
#include "hx2a/cursor_on_key.hpp"
#include "hx2a/checked_cast.hpp"
#include "hx2a/v8.hpp"

#include "interviews/exception.hpp"
#include "interviews/ontology.hpp"
#include "interviews/payloads.hpp"

// Design notes:
// Questionnaires, localizations and interviews are relatively small, so we do not care much about making single passes
// on these data. Quadratic algorithmes are avoided, but multi pass ones are considered okay.
// That simplifies the code and makes it easier to read and maintain.

namespace interviews {

  static inline void inject_loop_operand(const the_stack& ts, language_t lang, ostringstream& oc, const question_begin_loop_r& qbl, const answer_r& loop_operand_answer){
    // Let's obtain the vector we are iterating upon. We need the localized answer, as the loop variable is
    // typically used in parametric text.
    localized_answer_data_r lad = loop_operand_answer->make_localized_answer_data(ts, lang);
    // Must serialize the localized answer data to inject the value in V8 code to calculate the JSON vector.
    json::ostream<> jo;
    node_traits<localized_answer_data>::payload_serialize(jo, lad.get());
    // Parsing it into a JSON value to be able to remove the polymorphic type tag.
    json::value v = json::value::read<no_line_count, pretty>(jo.str());
    HX2A_ASSERT(v.if_object());
    const json::object_type& vobj = *v.if_object();
    // Only one key, the dollar-prefixed type tag. The base answer data type is never
    // instantiated.
    HX2A_ASSERT(vobj.size() == 1);
    HX2A_ASSERT(vobj.cbegin()->first.size() != 0);
    HX2A_ASSERT(vobj.cbegin()->first[0] == '$');
    // Removal of the '$' key from the JSON value.
    v = vobj.cbegin()->second;
    HX2A_LOG(trace) << "Injecting variable \"" << lad->_label << "\" with value " << v << " for a loop operand.";
    oc << "let " << lad->_label.get() << '=' << v << ';' << qbl->get_operand() << ';'; 
  }
  
  static inline json::value compute_loop_operand(const the_stack& ts, language_t lang, const question_begin_loop_r& qbl, const answer_r& loop_operand_answer){
    ostringstream oc;
    // Remember that undefined is not parsed as a result from a V8 call. We must not return it. Hence the complexity on testing
    // undefined.
    // let because we're compiling the code in a strict way and var would pollute the globals, as we reuse V8's heap.
    // Opening a brace for the same.
    oc << "{let R=null;";
    inject_loop_operand(ts, lang, oc, qbl, loop_operand_answer);
    // We are paranoid, the loop operand calculation might have assigned undefined to R.
    oc << "if(R==undefined){null}else R}";
    return v8_execute(oc.str());
  }
  
  json::value the_stack_frame::calculate_loop_operand(const the_stack& ts, language_t lang) const {
    return compute_loop_operand(ts, lang, _question_begin_loop, _loop_operand_answer);
  }
  
  json::value the_stack_frame::calculate_loop_variable_value(const the_stack& ts, language_t lang, const question_begin_loop_r& qbl, const answer_r& loop_operand_answer, size_t index){
    ostringstream oc;
    // Remember that undefined is not parsed as a result from a V8 call. We must not return it. Hence the complexity on testing
    // undefined.
    // let because we're compiling the code in a strict way and var would pollute the globals, as we reuse V8's heap.
    // Opening a brace for the same.
    oc << "{let R=null;";
    inject_loop_operand(ts, lang, oc, qbl, loop_operand_answer);
    // We are paranoid, the loop operand calculation might have assigned undefined to R.
    oc << "if(R==undefined){null}else{R=R[" << index << "];if(R==undefined){null}else R}}";
    return v8_execute(oc.str());
  }
  
  json::value function::call(language_t lang){
    _code << js_variable(js_language_var, (double) lang);
    const language::info* i = language::get_info(lang);

    // It should have been validated before, oh well, belt and suspenders does not harm.
    if (i){
      _code << js_variable(js_language_str2_var, i->string_code_1);
    }

    return slot_js_run(_code);
  }
  
  question_p transition::run(const the_stack& ts, time_t start_timestamp){
    if (!_condition || _condition->empty()){
      HX2A_LOG(trace) << "Transition condition is empty.";
      return _destination;
    }
    
    HX2A_LOG(trace) << "Evaluating the condition \"" << _condition->get_code() << "\" of a transition.";
    
    // Injecting all the variables. The questions labels are the variable names. We've already
    // checked that they are acceptable for JavaScript.
    auto i = _condition->parameters_cbegin();
    auto e = _condition->parameters_cend();
    
    while (i != e){
      question_p if_q = *i;
      HX2A_ASSERT(if_q);
      question_r q = *if_q;
      
      if (answer_p if_a = ts.find_answer(q)){
	answer_r a = *if_a;
	// The questionnaire might have skipped the answer, in that case the parameter will be set to null.
	answer_data_r ad = a->make_answer_data(start_timestamp);
	// Serializing the answer data as a payload. Reusing the same type for downloading
	// interviews guarantees that downloaded interviews and conditions operate on the
	// exact same data.
	json::ostream<> jo;
	node_traits<answer_data>::payload_serialize(jo, ad.get());
	// Parsing it into a JSON value to be able to remove the polymorphic type tag.
	json::value v = json::value::read<no_line_count, pretty>(jo.str());
	HX2A_ASSERT(v.if_object());
	const json::object_type& vobj = *v.if_object();
	// Only one key, the dollar-prefixed type tag. The base answer data type is never
	// instantiated.
	HX2A_ASSERT(vobj.size() == 1);
	HX2A_ASSERT(vobj.cbegin()->first.size() != 0);
	HX2A_ASSERT(vobj.cbegin()->first[0] == '$');
	// Removal of the '$' key from the JSON value.
	v = vobj.cbegin()->second;
	HX2A_LOG(trace) << "Injecting variable \"" << q->get_label() << "\" with value " << v << " for a transition condition.";
	_condition->push_argument(q->get_label(), v);
      }
      else{
	_condition->push_argument(q->get_label(), json::value());
      }
      
      ++i;
    }

    json::value v = _condition->call();
    
    if (json::is_true(v)){
      return _destination;
    }
    
    return {};
  }

  bool question::validate_label(const string& label){
    static regex_t r("[a-zA-Z$][0-9a-zA-Z_$]*");
    return hx2a::regex::match(label, r) && !label_is_reserved(label);
  }

  // To clone only the transitions.
  // Passing the map that allows to find the new transitions destinations.
  void question::clone_transitions_to(const question_r& cq, const cloned_to_clone_questions_map& m){
    for (const auto& t: _transitions){
      HX2A_ASSERT(t);
      question_r d = t->get_destination();
      auto i = m.find(&d.get());
      HX2A_ASSERT(i != m.cend());
      HX2A_ASSERT(i->second);
      cq->push_transition_back(t->clone(*i->second));
    }
  }

  question_r question::run_transitions(const the_stack& ts, time_t start_timestamp) const {
    for (const auto& t: _transitions){
      HX2A_ASSERT(t);

      if (question_p q = t->run(ts, start_timestamp)){
	return *q;
      }
    }

    // The only question which has no catch all is the final one and we're not supposed to run its transitions.
    HX2A_ASSERT(false);
    throw internal_error();
  }

  void question::add_transitions_to(const source_question_r& sq) const {
    for (const auto& tran: _transitions){
      HX2A_ASSERT(tran);
      sq->_transitions.push_back(make<source_transition>(*tran));
    }
  }

  // Dummy implementation.
  source_question_r question::make_source_question(language_t) const {
    HX2A_ASSERT(false);
    return make<source_question>("");
  }

  bool question_from_template::can_be_final() const {
    return _template_question->can_be_final();    
  }
  
  source_question_r question_from_template::make_source_question(language_t lang) const {
    template_question_r tq = get_template_question();
    // The localization might be in the template library. Let's look for it.
    template_question_localization_p tql = template_question_localization::find(tq, lang);

    if (!tql){
      HX2A_LOG(error) << "Found a question with label \"" << get_label() << "\", which is a template question, and its localization is missing from the template library.";
      throw internal_error();
    }
	
    return (*tql)->make_source_question(*this);
  }

  source_question_r question_begin_loop::make_source_question(language_t) const {
    HX2A_ASSERT(_operand_question);
    return make<source_question_begin_loop>(get_label(), _operand_question->get_label(), _variable, _operand);
  }
  
  source_question_r question_end_loop::make_source_question(language_t) const {
    return make<source_question_end_loop>(get_label());
  }
  
  void template_question::update(const string& label){
    if (_label != label){
      if (template_question_p tq = find(*get_home(), label)){
	throw template_question_already_exists();
      }

      // To avoid testing again for equality.
      _label.set<passive, !check_different>(label);
    }
  }

  string question_body::calculate_text(
				       const string& label,
				       const the_stack& ts,
				       language_t lang,
				       const string& text
				       ) const {
    // Checking if we are in the most common case, to process it quickly.
    if (_text_functions.empty() && ts.empty()){
      HX2A_LOG(trace) << "No functions or loop variables.";
      return text;
    }

    HX2A_LOG(trace) << "We have a function or loop variable. Processing \"" << text << "\".";
    
    // Keeping the function call values somewhere in case they are used several times.
    ::std::vector<::std::optional<json::value>> function_call_values(_text_functions.size());
    // This is the calculated text stream.
    ostringstream s;
    
    auto i = text.cbegin();
    auto e = text.cend();

    while (i != e){
      char c = *i;
	
      if (c == eval_prefix){
	++i;

	if (i == e){
	  s << eval_prefix;
	  break;
	}

	c = *i;
	
	if (c == eval_open){
	  // Read @{, need to accumulate the function number or the loop variable name.
	  ++i;

	  if (i == e){
	    s << eval_open;
	    break;
	  }

	  c = *i;

	  if (isdigit(c)){
	    // Just in case we abort, we keep a "side" stream going.
	    bool abort = false;
	    ostringstream ss;
	    ss << eval_prefix << eval_open << c;
	    uint64_t funcn = c - '0';
	    
	    do {
	      ++i;
	      
	      if (i == e){
		abort = true;
		break;
	      }
	      
	      c = *i;

	      if (!isdigit(c)){
		abort = (c != eval_close);
		break;
	      }

	      ss << c;
	      // We let funny things happen in case of overflow. We do not check.
	      funcn += funcn * 10 + c - '0';
	    } while (true);

	    if (abort){
	      s << ss.str();
	      break;
	    }

	    HX2A_ASSERT(c ==  eval_close);

	    // Probably already checked, belt and suspenders.
	    if (funcn >= _text_functions.size()){
	      throw function_call_out_of_bounds(label);
	    }
	    
	    // Must call the function and insert the result in the stream.
	    if (function_call_values[funcn]){
	      // The function was already called, let's reuse the result.
	      s << *function_call_values[funcn];
	    }
	    else{
	      // We must push the arguments, call the function, record the result, and insert it.
	      function_p if_func = _text_functions[funcn];
	      HX2A_ASSERT(if_func);
	      function_r func = *if_func;
	      
	      // Inserting the arguments. Using the localized answer data, which are richer than mere answer data.
	      auto i = func->parameters_cbegin();
	      auto e = func->parameters_cend();
	      
	      while (i != e){
		question_p if_q = *i;
		HX2A_ASSERT(if_q);
		question_r q = *if_q;
		
		if (answer_p a = ts.find_answer(q)){
		  // The questionnaire might have skipped the answer, in that case the parameter will be set to null.
		  // Using localized answer data so that localized labels can be used in the JavaScript code and serialized in the
		  // parametric text.
		  localized_answer_data_r lad = (*a)->make_localized_answer_data(ts, lang);
		  // Serializing the localized answer data as a payload. Reusing the same type for downloading
		  // interviews guarantees that downloaded interviews and conditions operate on the exact same data.
		  json::ostream<> jo;
		  node_traits<localized_answer_data>::payload_serialize(jo, lad.get());
		  // Parsing it into a JSON value to be able to remove the polymorphic type tag.
		  json::value v = json::value::read<no_line_count, pretty>(jo.str());
		  HX2A_ASSERT(v.if_object());
		  const json::object_type& vobj = *v.if_object();
		  // Only one key, the dollar-prefixed type tag. The base answer data type is never
		  // instantiated.
		  HX2A_ASSERT(vobj.size() == 1);
		  HX2A_ASSERT(vobj.cbegin()->first.size() != 0);
		  HX2A_ASSERT(vobj.cbegin()->first[0] == '$');
		  // Removal of the '$' key from the JSON value.
		  v = vobj.cbegin()->second;
		  HX2A_LOG(trace) << "Injecting variable \"" << q->get_label() << "\" with value " << v << " for a parametric text.";
		  func->push_argument(q->get_label(), v);
		}
		else{
		  func->push_argument(q->get_label(), json::value());
		}
		
		++i;
	      } // End while.

	      {
		// We could also add loop variables by passing the stack.
		json::value v = func->call(lang);
	      
		// If it is a string, we must trim the double quotes around.
		if (const string* str = v.if_string()){
		  s << *str;
		}
		else{
		  s << v;
		}

		// Recording the returned value of the call to reuse it if necessary.
		function_call_values[funcn] = std::move(v);
	      }
	    }
	  } // End if (isdigit(c)).
	  else{
	    // It seems we're starting a loop variable access.
	    ostringstream vars;
	    vars << c;
	    
	    do {
	      ++i;
	      
	      if (i == e){
		break;
	      }
	      
	      c = *i;

	      if (c == eval_close){
		break;
	      }

	      // We let funny things happen in case of overflow. We do not check.
	      vars << c;
	    } while (true);

	    if (i == e){
	      s << eval_prefix << eval_open << vars.str();
	      break;
	    }

	    HX2A_ASSERT(c == eval_close);
	    string var(vars.str());
	    HX2A_LOG(trace) << "Trying to find the loop variable " << var << " value.";
	    json::value v = ts.get_loop_variable(lang, var);

	    if (!v){
	      throw question_loop_variable_unknown(label);
	    }
	    
	    if (const string* str = v.if_string()){
	      s << *str;
	    }
	    else{
	      s << v;
	    }
	  }
	} // End if (c == eval_open).
	else{
	  s << c;
	}
      }
      else{
	s << c;
      }
      
      ++i;
    } // End while.
    
    return string(s.str());
  }

  template_question_p template_question::find(const db::connector& cn, const string& label){
    // Check for unicity, attempt to get 2 rows.
    cursor c = cursor_on_key<template_question>(cn->get_index(config_name<"tq_l">), {.key = {label}, .limit = 2});
    c.read_next();
    const auto& r = c.get_rows();
    const size_t rows_count = r.size();
    
    if (!rows_count){
      return {};
    }
    
    if (rows_count != 1){
      // Just report into log, return first one.
      HX2A_LOG(error) << "Found more than one template question with label " << label << '.';
    }
    
    return r.front().get_doc();
  }
  
  void questionnaire::check() const {
    if (is_locked()){
      return;
    }
    
    // In case additional checks are necessary, they should be more streamlined, in a single pass.
    check_orphans();
  }
  
  void questionnaire::check_orphans() const {
    auto i = _questions.cbegin();
    auto e = _questions.cend();

    if ((i == e) || (_questions.size() == 1)){
      // Empty questionnaire, or only one question, we're good.
      return;
    }

    // Let's put all first question transitions destinations in a set.
    std::set<question*> questions_set;
    question_p if_first = *i;
    HX2A_ASSERT(if_first);
    question_r first = *if_first;
    
    auto ti = first->transitions_cbegin();
    auto te = first->transitions_cend();

    while (ti != te){
      transition_p if_t = *ti;
      HX2A_ASSERT(if_t);
      transition_r t = *if_t;
      
      // Harmless if the destination is already present, which is a situation that might happen.
      questions_set.insert(&t->get_destination().get());
      ++ti;
    }

    // Moving on to the second question.
    ++i;
    
    do {
      question_p if_q = *i;
      HX2A_ASSERT(if_q);
      question_r q = *if_q;

      if (questions_set.find(&q.get()) == questions_set.cend()){
	throw question_is_orphan(q->get_label());
      }
      
      // Let's put all the question transitions destinations in the set.
      ti = q->transitions_cbegin();
      te = q->transitions_cend();

      while (ti != te){
	transition_p if_t = *ti;
	HX2A_ASSERT(if_t);
	transition_r t = *if_t;
	
	// Harmless if the destination is already present, which is a situation that might happen.
	questions_set.insert(&t->get_destination().get());
	++ti;
      }

      ++i;
    } while (i != e);

    // We're all good!
  }

  // The process is in two passes to respect referential integrity. RI will not let transitions point
  // at destination questions which are not yet in the questionnaire clone.
  // First pass: the questions are cloned, without their transitions. At the end all destination questions
  // are in the clone questionnaire.
  // Second pass: the transitions are cloned and the destinations are assigned.
  questionnaire_r questionnaire::clone(
				       const db::connector& c,
				       const string& new_code,
				       const string& new_name,
				       const string& new_logo
				       ) const {
    questionnaire_r rtnd = make<questionnaire>(c, new_code, new_name, new_logo);
    // Let's keep a mapping between cloned questions and clone questions.
    cloned_to_clone_questions_map cloned_to_clone_questions;

    // First pass, let's clone all the questions.
    for (const question_p& if_q: _questions){
      HX2A_ASSERT(if_q);
      question_r q = *if_q;
      question_r cq = q->clone();
      rtnd->push_question_back(cq);
      HX2A_ASSERT(cloned_to_clone_questions.find(&q.get()) == cloned_to_clone_questions.cend());
      cloned_to_clone_questions[&q.get()] = &cq.get();
    }

    // Second pass, let's clone all the transitions.
    auto qi = _questions.cbegin();
    auto qe = _questions.cend();
    auto cqi = rtnd->questions_begin();
      
    while (qi != qe){
      HX2A_ASSERT(*qi);
      HX2A_ASSERT(*cqi);
      (*qi)->clone_transitions_to(*(*cqi), cloned_to_clone_questions);
      ++qi;
      ++cqi;
    }
    
    return rtnd;
  }

  void questionnaire::dump(question_infos_by_label_map& m){
    size_t qn = 0;
    loop_nest ln;

    for (const auto& q: _questions){
      if (ln.empty()){
	m.emplace(q->get_label(), pair{question_info(qn, {}, {}), q.get()});
	// Will throw if it is an end loop.
	q->update_loop_nest(ln);
      }
      else if (q->get_loop_type() == question::end_loop){
	question_begin_loop_p mbl = ln.back();
	ln.pop_back(); // We know how to update the loop nest.
	m.emplace(q->get_label(), pair{question_info(qn, ln, mbl), q.get()});
      }
      else{
	m.emplace(q->get_label(), pair{question_info(qn, ln, {}), q.get()});
	q->update_loop_nest(ln);
      }
      
      ++qn;
    }
  }

  void questionnaire::dump(leveled_questionnaire& lq){
    size_t qn = 0;
    loop_nest ln;

    for (const auto& q: _questions){
      if (ln.empty()){
	lq.emplace(q.get(), question_info(qn, ln, {}));
	// Will throw if it is an end loop.
	q->update_loop_nest(ln);
      }
      else if (q->get_loop_type() == question::end_loop){
	question_begin_loop_p mbl = ln.back();
	ln.pop_back(); // We know how to update the loop nest.
	lq.emplace(q.get(), question_info(qn, ln, mbl));
      }
      else{
	lq.emplace(q.get(), question_info(qn, ln, {}));
	q->update_loop_nest(ln);
      }
      
      ++qn;
    }
  }

  source_template_question_r question_localization_body::make_source_template_question(const template_question_localization_r& tql) const {
    HX2A_ASSERT(false);
    return make<source_template_question>(tql->get_language(), "", "", "");
  }
  
  source_template_question_localization_r question_localization_body::make_source_template_question_localization(const template_question_localization_r& tql) const {
    HX2A_ASSERT(false);
    return make<source_template_question_localization>(tql->get_template_question(), tql->get_language(), "");
  }
  
  localized_question_r question_localization_body::make_localized_question(
									   const string&,
									   const the_stack&,
									   language_t,
									   const string&,
									   const string&,
									   const question_r&,
									   percentage_t /* progress */
									   ) const {
    HX2A_ASSERT(false);
    return make<localized_question>("", "", "", "", "", 0);
  }

  source_question_r question_localization_body_message::make_source_question(const question_r& q) const {
    auto qb = checked_cast<question_body_message>(q->get_body());
    return make<source_question_message>(q->get_label(), q->get_body()->get_style(), make<source_text>(qb, *this));
  }

  source_template_question_r question_localization_body_message::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    return make<source_template_question_message>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text());
  }

  source_template_question_localization_r question_localization_body_message::make_source_template_question_localization(const template_question_localization_r& tql) const {
    return make<source_template_question_localization_message>(tql->get_template_question(), tql->get_language(), get_text());
  }

  localized_question_r question_localization_body_message::make_localized_question(
										   const string& label,
										   const the_stack& ts,
										   language_t lang,
										   const string& logo,
										   const string& title,
										   const question_r& q,
										   percentage_t progress
										   ) const {
    bool is_final = !q->transitions_size();
    question_body_r qb = q->get_body();
    return make<localized_question_message>(q->get_label(), logo, title, qb->get_style(), is_final, calculate_text(label, ts, lang, qb), is_final ? 100 : progress);
  }
  
  void question_localization_body_input::check_more(const string& label, const question_body_r&) const {
    if (get_text().empty()){
      throw question_localization_text_is_missing(label);
    }
  }
  
  source_question_r question_localization_body_input::make_source_question(const question_r& q) const {
    auto qbi = checked_cast<question_body_input>(q->get_body());
    return make<source_question_input>(q->get_label(), qbi->get_style(), make<source_text>(qbi, *this), get_comment_label(), qbi->is_optional());
  }
 
  source_template_question_r question_localization_body_input::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbi = checked_cast<question_body_input>(tq->get_body());
    return make<source_template_question_input>(tql->get_language(), tq->get_label(), qbi->get_style(), get_text(), get_comment_label(), qbi->is_optional());
  }

  source_template_question_localization_r question_localization_body_input::make_source_template_question_localization(const template_question_localization_r& tql) const {
    return make<source_template_question_localization_input>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
  }

  localized_question_r question_localization_body_input::make_localized_question(
										 const string& label,
										 const the_stack& ts,
										 language_t lang,
										 const string& logo,
										 const string& title,
										 const question_r& q,
										 percentage_t progress
										 ) const {
    auto qbi = checked_cast<question_body_input>(q->get_body());
    return make<localized_question_input>(q->get_label(), logo, title, qbi->get_style(), calculate_text(label, ts, lang, qbi), progress, get_comment_label(), qbi->is_optional());
  }
  
  void question_localization_body_with_options::add_options_to(const source_template_question_with_options_r& to) const {
    for (const auto& c: _options){
      HX2A_ASSERT(c);
      to->_options.push_back(make<source_option>(*c));
    }
  }

  void question_localization_body_with_options::add_options_to(const source_template_question_localization_with_options_r& to) const {
    for (const auto& c: _options){
      HX2A_ASSERT(c);
      to->_options.push_back(make<source_option_localization>(*c));
    }
  }

  option_localization_r question_localization_body_with_options::find_option_localization(size_t index) const {
    if (_options.size() <= index){
      throw selection_is_invalid();
    }

    for (const auto& cl: _options){
      HX2A_ASSERT(cl);
      
      if (!index){
	return *cl;
      }

      --index;
    }

    // Can't happen.
    HX2A_ASSERT(false);
    throw 0;
  }

  option_localization_r question_localization_body_with_options::find_option_localization(const option_r& op) const {
    auto i =
      std::find_if(_options.cbegin(), _options.cend(), [&](const option_localization_p& ol){
							 HX2A_ASSERT(ol);
							 return (*ol)->get_option() == op;
						       });

    if (i != _options.cend()){
      HX2A_ASSERT(*i);
      return **i;
    }
    
    HX2A_LOG(error) << "Cannot find option localization.";
    throw internal_error();
  }

  void question_localization_body_with_options::check_more(const string& label, const question_body_r& qb) const {
    auto qbwo = checked_cast<question_body_with_options>(qb);

    if (_options.size() != qbwo->get_options_size()){
      throw question_localization_options_size_is_incorrect(label);
    }
  }

  static inline void add_options_to_localized_question(const rfr<question_localization_body_with_options>& qlbo, const rfr<localized_question_with_options>& lq){
    auto i = qlbo->options_cbegin();
    auto e = qlbo->options_cend();
    
    while (i != e){
      option_localization_p if_ol = *i;
      HX2A_ASSERT(if_ol);
      option_localization_r ol = *if_ol;
      lq->_options.push_back(make<source_option>(ol->get_label(), ol->get_comment_label()));
      ++i;
    }
  }
    
  source_question_r question_localization_body_select::make_source_question(const question_r& q) const {
    auto qbwo = checked_cast<question_body_with_options>(q->get_body());
    rfr<source_question_select> rtnd = make<source_question_select>(q->get_label(), qbwo->get_style(), make<source_text>(qbwo, *this), get_comment_label(), qbwo->get_randomize());
    auto i = options_cbegin();
    auto e = options_cend();
    
    while (i != e){
      option_localization_p if_ol = *i;
      HX2A_ASSERT(if_ol);
      option_localization_r ol = *if_ol;
      rtnd->_options.push_back(make<source_option>(ol->get_label(), ol->get_comment_label()));
      ++i;
    }
    
    return rtnd;
  }

  source_template_question_r question_localization_body_select::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbwo = checked_cast<question_body_with_options>(tq->get_body());
    auto stqs = make<source_template_question_select>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text(), get_comment_label(), qbwo->get_randomize());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqs);
    return stqs;
  }

  source_template_question_localization_r question_localization_body_select::make_source_template_question_localization(const template_question_localization_r& tql) const {
    auto stqls = make<source_template_question_localization_select>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqls);
    return stqls;
  }

  localized_question_r question_localization_body_select::make_localized_question(
										  const string& label,
										  const the_stack& ts,
										  language_t lang,
										  const string& logo,
										  const string& title,
										  const question_r& q,
										  percentage_t progress
										  ) const {
    question_body_r qb = q->get_body();
    rfr<localized_question_with_options> rtnd = make<localized_question_select>(q->get_label(), logo, title, qb->get_style(), calculate_text(label, ts, lang, qb), progress, get_comment_label());
    add_options_to_localized_question(*this, rtnd);
    return rtnd;
  }
  
  source_question_r question_localization_body_select_at_most::make_source_question(const question_r& q) const {
    return tmpl_make_source_question<source_question_select_at_most>(q);
  }

  source_template_question_r question_localization_body_select_at_most::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbmc = checked_cast<question_body_multiple_choices>(tq->get_body());
    auto stqs = make<source_template_question_select_at_most>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text(), get_comment_label(), qbmc->get_randomize(), qbmc->get_limit());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqs);
    return stqs;
  }

  source_template_question_localization_r question_localization_body_select_at_most::make_source_template_question_localization(const template_question_localization_r& tql) const {
    auto stqls = make<source_template_question_localization_select_at_most>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqls);
    return stqls;
  }

  localized_question_r question_localization_body_select_at_most::make_localized_question(
											  const string& label,
											  const the_stack& ts,
											  language_t lang,
											  const string& logo,
											  const string& title,
											  const question_r& q,
											  percentage_t progress
											  ) const {
    auto qbmc = checked_cast<question_body_multiple_choices>(q->get_body());
    rfr<localized_question_with_options> rtnd = make<localized_question_select_at_most>(q->get_label(), logo, title, qbmc->get_style(), calculate_text(label, ts, lang, qbmc), progress, get_comment_label(), qbmc->get_limit());
    add_options_to_localized_question(*this, rtnd);
    return rtnd;
  }
  
  source_question_r question_localization_body_select_limit::make_source_question(const question_r& q) const {
    return tmpl_make_source_question<source_question_select_limit>(q);
  }

  source_template_question_r question_localization_body_select_limit::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbmc = checked_cast<question_body_multiple_choices>(tq->get_body());
    auto stqs = make<source_template_question_select_limit>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text(), get_comment_label(), qbmc->get_randomize(), qbmc->get_limit());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqs);
    return stqs;
  }

  source_template_question_localization_r question_localization_body_select_limit::make_source_template_question_localization(const template_question_localization_r& tql) const {
    auto stqls = make<source_template_question_localization_select_limit>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqls);
    return stqls;
  }

  localized_question_r question_localization_body_select_limit::make_localized_question(
											const string& label,
											const the_stack& ts,
											language_t lang,
											const string& logo,
											const string& title,
											const question_r& q,

											percentage_t progress) const {
    auto qbmc = checked_cast<question_body_multiple_choices>(q->get_body());
    rfr<localized_question_with_options> rtnd = make<localized_question_select_limit>(q->get_label(), logo, title, qbmc->get_style(), calculate_text(label, ts, lang, qbmc), progress, get_comment_label(), qbmc->get_limit());
    add_options_to_localized_question(*this, rtnd);
    return rtnd;
  }
  
  source_question_r question_localization_body_rank_at_most::make_source_question(const question_r& q) const {
    return tmpl_make_source_question<source_question_rank_at_most>(q);
  }

  source_template_question_r question_localization_body_rank_at_most::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbmc = checked_cast<question_body_multiple_choices>(tq->get_body());
    auto stqs = make<source_template_question_rank_at_most>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text(), get_comment_label(), qbmc->get_randomize(), qbmc->get_limit());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqs);
    return stqs;
  }

  source_template_question_localization_r question_localization_body_rank_at_most::make_source_template_question_localization(const template_question_localization_r& tql) const {
    auto stqls = make<source_template_question_localization_rank_at_most>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqls);
    return stqls;
  }

  localized_question_r question_localization_body_rank_at_most::make_localized_question(
											const string& label,
											const the_stack& ts,
											language_t lang,
											const string& logo,
											const string& title,
											const question_r& q,
											percentage_t progress
											) const {
    auto qbmc = checked_cast<question_body_multiple_choices>(q->get_body());
    rfr<localized_question_with_options> rtnd = make<localized_question_rank_at_most>(q->get_label(), logo, title, qbmc->get_style(), calculate_text(label, ts, lang, qbmc), progress, get_comment_label(), qbmc->get_limit());
    add_options_to_localized_question(*this, rtnd);
    return rtnd;
  }
  
  source_question_r question_localization_body_rank_limit::make_source_question(const question_r& q) const {
    return tmpl_make_source_question<source_question_rank_limit>(q);
  }

  source_template_question_r question_localization_body_rank_limit::make_source_template_question(const template_question_localization_r& tql) const {
    template_question_r tq = tql->get_template_question();
    auto qbmc = checked_cast<question_body_multiple_choices>(tq->get_body());
    auto stqs = make<source_template_question_rank_limit>(tql->get_language(), tq->get_label(), tq->get_body()->get_style(), get_text(), get_comment_label(), qbmc->get_randomize(), qbmc->get_limit());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqs);
    return stqs;
  }

  source_template_question_localization_r question_localization_body_rank_limit::make_source_template_question_localization(const template_question_localization_r& tql) const {
    auto stqls = make<source_template_question_localization_rank_limit>(tql->get_template_question(), tql->get_language(), get_text(), get_comment_label());
    auto qlbwo = checked_cast<question_localization_body_with_options>(tql->get_body());
    qlbwo->add_options_to(stqls);
    return stqls;
  }

  localized_question_r question_localization_body_rank_limit::make_localized_question(
										      const string& label,
										      const the_stack& ts,
										      language_t lang,
										      const string& logo,
										      const string& title,
										      const question_r& q,
										      percentage_t progress
										      ) const {
    auto qbmc = checked_cast<question_body_multiple_choices>(q->get_body());
    rfr<localized_question_with_options> rtnd = make<localized_question_rank_limit>(q->get_label(), logo, title, qbmc->get_style(), calculate_text(label, ts, lang, qbmc), progress, get_comment_label(), qbmc->get_limit());
    add_options_to_localized_question(*this, rtnd);
    return rtnd;
  }

  source_question_r template_question_localization::make_source_question(const question_from_template_r& q) const {
    HX2A_ASSERT(_body);
    HX2A_ASSERT(_template_question);
    auto sqft = make<source_question_from_template>(q->get_label(), _template_question->get_label());
    q->add_transitions_to(sqft);
    return sqft;
  }
  
  template_question_localization_p template_question_localization::find(const template_question_r& tq, language_t lang){
    // Check for unicity, attempt to get 2 rows.
    cursor c = cursor_on_key<template_question_localization>(tq->get_home()->get_index(config_name<"tql_q">), {.key = {tq->get_id(), lang}, .limit = 2});
    c.read_next();
    const auto& r = c.get_rows();
    const size_t rows_count = r.size();
    
    if (!rows_count){
      return {};
    }
    
    if (rows_count != 1){
      // Just report into log, return first one.
      HX2A_LOG(error) << "Found more than one template question localization for template question doc id " << tq->get_id() << " and language code " << lang << ". Retaining the first one found.";
    }
    
    return r.front().get_doc();
  }
  
  void questionnaire_localization::dump(questionnaire_localization_map_per_question& m) const {
    auto e = m.cend();
    
    for (const auto& ql: _questions_localizations){
      question_r q = ql->get_question();
      // Checking redundancy.
      auto f = m.find(&q.get());

      if (f != e){
	throw question_localization_is_duplicate(q->get_label());
      }
      
      m[&q.get()] = ql;
    }
  }
  
  void questionnaire_localization::check(){
    // If we have already checked the localization for that version of the questionnaire, we do not need to do it again.
    if (_questionnaire_change_count == _questionnaire->get_change_count()){
      // Already done.
      return;
    }

    force_check();
    // Noting that the check was already done for that version of the questionnaire.
    _questionnaire_change_count = _questionnaire->get_change_count();
  }

  // This function checks a number of properties of a questionnaire localization, namely:
  // - That the localization is complete, meaning that it covers the entirety of the questionnaire. No localization is
  //   missing. The process identifies questions from templates and verifies that a localization for the corresponding language
  //   exists in the template library.
  //   To resist to disruptions in the template library, localizations in the template library can be created but cannot
  //   be removed once created. They can only be updated. That way, once a questionnaire localization is checked successfully
  //   it remains good.
  // - That the localization is not redundant. No question has two localizations.
  void questionnaire_localization::force_check() const {
    // Because there might be automatically borrowed localizations from the template library, we cannot just test the size of the
    // questionnaire localization vs. the size of the questionnaire to check that there is no localization missing.

    // Checking completeness.
    // Scanning all questions in the questionnaire, and checking they have their corresponding localization, either in the
    // questionnaire localization or in the template library.
    {
      questionnaire_localization_map_per_question qlmpq;
      // This will check redundancy.
      dump(qlmpq);
      questionnaire_r quest = get_questionnaire();
      auto i = quest->questions_cbegin();
      auto e = quest->questions_cend();
      auto me = qlmpq.cend();

      // Looping over all questions in the questionnaire.
      while (i != e){
	// Checking that the question has a localization, either in the questionnaire localization or in the template library.
	question_p if_q = *i;
	HX2A_ASSERT(if_q);
	question_r q = *if_q;
	
	if (qlmpq.find(&q.get()) == me){
	  // Could not find the question in the questionnaire localization, let's check if the question supports a localization.
	  if (q->supports_localization()){
	    throw question_localization_does_not_exist(q->get_label());
	  }

	  // Let's see if the question is from template to see if the localization exists in the template library.
	  question_from_template* qft = dynamic_cast<question_from_template*>(&q.get());

	  if (qft){
	    // It is a question from template, let's switch to the template library.
	    template_question_localization_p tql = template_question_localization::find(qft->get_template_question(), get_language());
	    
	    if (!tql){
	      throw question_localization_for_template_does_not_exist(q->get_label());
	    }
	  }
	}
	
	++i;
      }
    }
  }

  questionnaire_localization_p questionnaire_localization::find(const questionnaire_r& q, language_t lang){
    // Check for unicity, attempt to get 2 rows.
    cursor c = cursor_on_key<questionnaire_localization>(q->get_home()->get_index(config_name<"qloc_q">), {.key = {q->get_id(), lang}, .limit = 2});
    c.read_next();
    const auto& r = c.get_rows();
    const size_t rows_count = r.size();

    if (!rows_count){
      return {};
    }
    
    if (rows_count != 1){
      // Just report into log, return first one.
      HX2A_LOG(error) << "Found more than one questionnaire localization for questionnaire doc id " << q->get_id() << " and language code " << lang << ". Retaining the first one found.";
    }

    return r.front().get_doc();
  }

  // Dummy.
  answer_data_r answer_body::make_answer_data(const answer_r&, time_t) const {
    HX2A_ASSERT(false);
    return make<answer_data>("", "", 0, 0, 0, nullptr);
  }
  
  // Dummy.
  localized_answer_data_r answer_body::make_localized_answer_data(const string& /* label */, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const {
    HX2A_ASSERT(false);
    return make<localized_answer_data>("", "");
  }

  // Specializations.
  
answer_data_r answer_body_message::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    return make<answer_data_message>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation());
  }
  
  localized_answer_data_r answer_body_message::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    return make<localized_answer_data_message>(label, qlb->calculate_text(label, ts, lang, qb));
  }

  answer_data_r answer_body_input::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    return make<answer_data_input>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), get_comment(), _input.get());
  }
  
  localized_answer_data_r answer_body_input::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwc = checked_cast<question_localization_body_with_comment>(qlb);
    return make<localized_answer_data_input>(label, qlb->calculate_text(label, ts, lang, qb), qlbwc->get_comment_label(), get_comment(), _input.get());
  }

  answer_data_r answer_body_select::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    HX2A_ASSERT(_choice);
    return make<answer_data_select>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), make<choice_payload>(*_choice), get_comment());
  }
  
  localized_answer_data_r answer_body_select::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    rfr<localized_answer_data_select> la = make<localized_answer_data_select>(label, qlb->calculate_text(label, ts, lang, qb), qlbwo->get_comment_label(), get_comment());
    HX2A_ASSERT(_choice);
    la->_choice = make<choice_payload>(*_choice);
    shared_add_options_to_localized_answer_data(qlbwo, la);
    return la;
  }

  void answer_body_multiple_choices::shared_add_options_to_answer_data(const answer_data_multiple_choices_r& ap) const {
    for (const auto& ch: _choices){
      HX2A_ASSERT(ch);
      ap->push_choice_back(make<choice_payload>(*ch));
    }
  }

  void answer_body_with_options::shared_add_options_to_localized_answer_data(const question_localization_body_with_options_r& qlbwo, const localized_answer_data_with_options_r& lad) const {
    auto i = qlbwo->options_cbegin();
    auto e = qlbwo->options_cend();
    
    while (i != e){
      option_localization_p if_ol = *i;
      HX2A_ASSERT(if_ol);
      option_localization_r ol = *if_ol;
      lad->_options.push_back(make<source_option>(ol->get_label(), ol->get_comment_label()));
      ++i;
    }
  }
    
  // Adds options AND choices to the argument.
  void answer_body_multiple_choices::shared_add_options_to_localized_answer_data(const question_localization_body_with_options_r& qlbwo, const localized_answer_data_multiple_choices_r& la) const {
    // Adding options.
    answer_body_with_options::shared_add_options_to_localized_answer_data(qlbwo, la);
    
    // Adding choices.
    for (const auto& ch: _choices){
      HX2A_ASSERT(ch);
      la->_choices.push_back(make<choice_payload>(*ch));
    }
  }

answer_data_r answer_body_select_at_most::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    answer_data_multiple_choices_r apb = make<answer_data_select_at_most>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), get_comment());
    // Must now take care of the options.
    shared_add_options_to_answer_data(apb);
    return apb;
  }
  
  localized_answer_data_r answer_body_select_at_most::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    rfr<localized_answer_data_select_at_most> la = make<localized_answer_data_select_at_most>(label, qlb->calculate_text(label, ts, lang, qb), qlbwo->get_comment_label(), get_comment());
    shared_add_options_to_localized_answer_data(qlbwo, la);
    return la;
  }
  
  answer_data_r answer_body_select_limit::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    answer_data_multiple_choices_r apb = make<answer_data_select_limit>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), get_comment());
    // Must now take care of the options.
    shared_add_options_to_answer_data(apb);
    return apb;
  }
  
  localized_answer_data_r answer_body_select_limit::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    rfr<localized_answer_data_select_limit> la = make<localized_answer_data_select_limit>(label, qlb->calculate_text(label, ts, lang, qb), qlbwo->get_comment_label(), get_comment());
    shared_add_options_to_localized_answer_data(qlbwo, la);
    return la;
  }
  
  answer_data_r answer_body_rank_at_most::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    rfr<answer_data_multiple_choices> apb = make<answer_data_rank_at_most>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), get_comment());
    // Must now take care of the options.
    shared_add_options_to_answer_data(apb);
    return apb;
  }
  
  localized_answer_data_r answer_body_rank_at_most::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    rfr<localized_answer_data_rank_at_most> la = make<localized_answer_data_rank_at_most>(label, qlb->calculate_text(label, ts, lang, qb), qlbwo->get_comment_label(), get_comment());
    shared_add_options_to_localized_answer_data(qlbwo, la);
    return la;
  }
  
  answer_data_r answer_body_rank_limit::make_answer_data(const answer_r& a, time_t start_timestamp) const {
    answer_data_multiple_choices_r apb = make<answer_data_rank_limit>(a->get_label(), a->get_ip_address(), a->get_timestamp(start_timestamp), a->get_elapsed(), a->get_total_elapsed(), a->get_geolocation(), get_comment());
    // Must now take care of the options.
    shared_add_options_to_answer_data(apb);
    return apb;
  }
  
  localized_answer_data_r answer_body_rank_limit::make_localized_answer_data(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    rfr<localized_answer_data_rank_limit> la = make<localized_answer_data_rank_limit>(label, qlb->calculate_text(label, ts, lang, qb), qlbwo->get_comment_label(), get_comment());
    shared_add_options_to_localized_answer_data(qlbwo, la);
    return la;
  }
  
  void interview::start(
			const string& interviewee_id,
			const string& interviewer_id,
			const user_p& interviewer_user,
			language_t language,
			string_view start_ip_address,
			const geolocation_p& geo
			){
    _start_ip_address = string{start_ip_address};
    _start_timestamp = time();
    _start_geolocation = geo;
    _interviewee_id = interviewee_id;
    _interviewer_id = interviewer_id;
    _interviewer_user = interviewer_user;
    _language = language;
    _questionnaire_localization = questionnaire_localization::find(_campaign->get_questionnaire(), language);

    if (!_questionnaire_localization){
      throw questionnaire_localization_does_not_exist();
    }
    
    _questionnaire_localization->check();

    if (_state != initiated){
      // We can only start an initiated interview.
      throw interview_is_already_started();
    }

    _state = ongoing;
    // The first question might be the final one.
    set_next_question(get_first_question());
  }

  question_r interview::run_transitions(const the_stack& ts, const question_r& q) const {
    return q->run_transitions(ts, _start_timestamp);
  }

  // If the campaign is over, an exception is raised.
  localizations interview::next_question_localization() const {
    HX2A_ASSERT(_next_question);
    HX2A_ASSERT(_next_question->supports_localization());

    if (question_localization_p ql = find_question_localization(*_next_question)){
      return *ql;
    }
      
    if (question_from_template* qft = dynamic_cast<question_from_template*>(_next_question.get())){
      return template_localization{*template_question_localization::find(qft->get_template_question(), get_language()), *qft};
    }

    HX2A_LOG(error) << "Cannot find a localization for the question with label " << _next_question->get_label();
    throw internal_error();
  }
    
  question_r interview::calculate_new_next_question(the_stack& ts){
    HX2A_ASSERT(_state == ongoing);
    HX2A_ASSERT(_next_question);
    HX2A_ASSERT(_next_question->supports_localization());
    // Let's run the transitions from the current next question.
    question_r new_next_question = run_transitions(ts, *_next_question);
    // Now the new next question might be a loop, so if it is not, we're fine, and if it is, we need to scan further.
    new_next_question = calculate_new_next_question(ts, new_next_question);
    set_next_question(new_next_question);
    return new_next_question;
  }
  
  question_r interview::calculate_new_next_question(the_stack& ts, const question_r& q){
    if (q->supports_localization()){
      return q;
    }

    if (question_begin_loop* qbl = dynamic_cast<question_begin_loop*>(&q.get())){
      return process_begin_loop(ts, *qbl);
    }
    
    if (question_end_loop* qel = dynamic_cast<question_end_loop*>(&q.get())){
      return process_end_loop(ts, *qel);
    }
    
    throw internal_error();
  }
  
  localized_question_r interview::move_ahead(){
    HX2A_ASSERT(_state == ongoing);
    
    the_stack ts;
    calculate(ts);
    ts.dump();

    question_r new_next_question = calculate_new_next_question(ts);
    question_localization_p if_ql = find_question_localization(new_next_question);
    questionnaire_localization_p if_qql = get_questionnaire_localization();
    HX2A_ASSERT(if_qql);
    questionnaire_localization_r qql = *if_qql;
    
    if (if_ql){
      question_localization_r ql = *if_ql;
      return ql->make_localized_question(ts, _language, get_questionnaire()->get_logo(), qql->get_title(), qql->get_progress(ql->get_question()));
    }
    
    if (question_from_template* qft = dynamic_cast<question_from_template*>(&new_next_question.get())){
      if (template_question_localization_p tql = template_question_localization::find(qft->get_template_question(), get_language())){
	return (*tql)->make_localized_question(qft->get_label(), ts, _language, get_questionnaire()->get_logo(), qql->get_title(), *qft, qql->get_progress(*qft));
      }
    }

    HX2A_LOG(error) << "Cannot find a localization for the question with label " << new_next_question->get_label();
    throw internal_error();
  }
  
  question_end_loop_p interview::find_matching_end_loop(const question_begin_loop_r& qbl) const {
    questionnaire_r qq = get_questionnaire();
    auto i = qq->questions_cbegin();
    auto e = qq->questions_cend();
    question_end_loop_p qel;
    
    // Let's first find the question begin loop.
    while (i != e){
      if ((*i) == qbl){
	// Found it.
	++i;
	HX2A_ASSERT(i != e);

	// Let's find the matching question end loop. We might encounter begin and end loops.
	size_t lc = 1; // We already have the begin.
	
	do {
	  question_p if_q = *i;
	  HX2A_ASSERT(if_q);
	  question_r q = *if_q;
	  // As begins and ends are well-balanced, we can simply use a counter, no need for something more complicated.
	  q->update_loop_counter(lc);

	  if (!lc){
	    // Found it.
	    qel = dynamic_cast<question_end_loop*>(&q.get());
	    HX2A_ASSERT(qel);
	    return qel;
	  }

	  ++i;
	} while (i != e);
      }

      ++i;
    }

    return qel;
  }

  // Function called when a question begin loop is encountered for the first time.
  question_r interview::process_begin_loop(the_stack& ts, const question_begin_loop_r& qbl){
    // We need to evaluate the first loop variable value, if any (the vector might be empty), to see if there is a point running
    // the loop, or if it is necessary to jump to the matching end.
    answer_p loa = ts.process_begin_loop(_language, qbl);
    
    if (loa){
      // Recording the begin loop in the interview.
      add_begin_loop(qbl, *loa, ts.get_index());
      // Processing transitions.
      question_r q = run_transitions(ts, qbl);
      // The question cannot be a begin loop, as begin loops are not allowed to transition to another begin loop. That other
      // begin loop has to operate over an answer of the same loop nest, so that answer has to be in between.
      // But the question might be an end loop.
      question_end_loop* qel = dynamic_cast<question_end_loop*>(&q.get());

      if (qel){
	return process_end_loop(ts, *qel);
      }
      
      return calculate_new_next_question(ts, q);
    }

    // Nothing to loop upon, we must go past the next question end loop.
    // Let's find it. There should be one. This situation is scarce, we can afford an expensive
    // operation. We scan the whole questionnaire up to the begin loop question we have. The
    // alternative to that would be to keep an iterator on the question begin loop. Expensive
    // also for a scarce case.
    question_end_loop_p qel = find_matching_end_loop(qbl);

    if (qel){
      question_r q = run_transitions(ts, *qel);
      
      // The question might be a loop one.
      switch (q->get_loop_type()){
      case question::regular:
	{
	  return calculate_new_next_question(ts, q);
	}

      case question::begin_loop:
	{
	  question_begin_loop* qbl = dynamic_cast<question_begin_loop*>(&q.get());
	  HX2A_ASSERT(qbl);
	  return process_begin_loop(ts, *qbl);
	}

      case question::end_loop:
	{
	  question_end_loop* qel = dynamic_cast<question_end_loop*>(&q.get());
	  HX2A_ASSERT(qel);
	  return process_end_loop(ts, *qel);
	}
      }
    }

    // The compilation of the source questionnaire guarantees that begin/ends are properly balanced.
    HX2A_ASSERT(false);
    throw internal_error();
  }

  question_r interview::process_end_loop(the_stack& ts, const question_end_loop_r& qel){
    // Two cases, either we must iterate or we must move to the next question. Note that transitions are allowed on an end
    // loop question (executed at the end of the loop only).
    
    // Let's get the current stack frame, if any.
    if (ts.empty()){
      throw question_loop_logic_error(qel->get_label());
    }

    // We add it unconditionally.
    add_end_loop(qel);
    
    if (ts.process_end_loop()){
      // Still iterating. Let's run the transitions of the begin loop.
      question_begin_loop_r qbl = ts.get_question_begin_loop();
      question_r q = run_transitions(ts, qbl);
      // The question cannot be a begin loop, as begin loops are not allowed to transition to another begin loop. That other
      // begin loop has to operate over an answer of the same loop nest, so that answer has to be in between.
      // But the question might be an end loop.
      question_end_loop* qel = dynamic_cast<question_end_loop*>(&q.get());

      if (qel){
	return process_end_loop(ts, *qel);
      }
      
      return calculate_new_next_question(ts, q);
    }

    // Loop is finished.
    // Let's run the transitions.
    question_r q = run_transitions(ts, qel);
    
    // The question might be a loop one.
    switch (q->get_loop_type()){
    case question::regular:
      {
	break;
      }
      
    case question::begin_loop:
      {
	question_begin_loop* qbl = dynamic_cast<question_begin_loop*>(&q.get());
	HX2A_ASSERT(qbl);
	return process_begin_loop(ts, *qbl);
      }
      
    case question::end_loop:
      {
	question_end_loop* qel = dynamic_cast<question_end_loop*>(&q.get());
	HX2A_ASSERT(qel);
	return process_end_loop(ts, *qel);
      }
    }
    
    return calculate_new_next_question(ts, q);
  }

  localized_question_r interview::next_localized_question(the_stack& ts) const {
    HX2A_ASSERT(_state != initiated);
    HX2A_ASSERT(_next_question);
    HX2A_ASSERT(_next_question->supports_localization());
    question_localization_p if_ql = find_question_localization(*_next_question);
    questionnaire_localization_p if_qql = get_questionnaire_localization();
    HX2A_ASSERT(if_qql);
    questionnaire_localization_r qql = *if_qql;
    
    if (if_ql){
      question_localization_r ql = *if_ql;
      return ql->make_localized_question(ts, _language, get_questionnaire()->get_logo(), qql->get_title(), qql->get_progress(ql->get_question()));
    }
    
    if (question_from_template* qft = dynamic_cast<question_from_template*>(_next_question.get())){
      template_question_localization_p tql = template_question_localization::find(qft->get_template_question(), get_language());

      if (!tql){
	HX2A_LOG(error) << "Found a question with label \"" << _next_question->get_label() << "\", which is a template question, and its localization is missing from the template library.";
	throw internal_error();
      }

      return (*tql)->make_localized_question(qft->get_label(), ts, _language, get_questionnaire()->get_logo(), qql->get_title(), *qft, qql->get_progress(*qft));
    }

    HX2A_LOG(error) << "Cannot find a localization for the question with label " << _next_question->get_label();
    throw internal_error();
  }

  // Calculating the stack up to the answer index specified, that answer excluded.
  void interview::calculate(the_stack& ts, history_type::const_iterator pos) const {
    HX2A_LOG(trace) << "Calculating the stack up to an answer index.";
    HX2A_ASSERT(!ts.size());
    
    if constexpr (debug_mode){
      // We check that the index pointed at a "real" answer, not a begin or end loop.
      if (pos != _history.cend()){
	HX2A_ASSERT(*pos);
	entry_r e = **pos;
	
	if (!dynamic_cast<entry_answer*>(&e.get())){
	  HX2A_LOG(trace) << "Answer index does not point at an answer.";
	  throw internal_error();
	}
      }
    }
    
    auto i = _history.cbegin();

    while (i != pos){
      HX2A_ASSERT(*i);
      entry_r e = **i;
      ts.process_entry(_language, e);
      ++i;
    }
  }

  static inline question_r find_next_regular_question(the_stack& ts, language_t lang, const question_r& q, time_t start_timestamp){
    question_r f = q;
    
    while (true){
      switch (f->get_loop_type()){
      case question::regular:
	{
	  return f;
	}
	
      case question::begin_loop:
	{
	  question_begin_loop* qbl = dynamic_cast<question_begin_loop*>(&f.get());
	  HX2A_ASSERT(qbl);
	  ts.process_begin_loop(lang, *qbl);
	  break;
	}
	
      case question::end_loop:
	{
	  ts.process_end_loop();
	  break;
	}
      }

      f = f->run_transitions(ts, start_timestamp);
    }
  }

  // The function will return true if the entry is truly impacted.
  // Otherwise it'll return false and will update the previous and new stacks.
  // Language given for the case where a text is impacted. Never know, different languages might have different impact status.
  // The previous and new answers given for the case where a begin loop is impacted and avoiding a search.
  static inline bool process_impacted_entry(the_stack& pts, the_stack& nts, const answer_r& ploa, const answer_r& nloa, language_t lang, const entry_r& e){
    entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());

    if (ea){
      // The only way for an answer to be impacted is that the question has a calculated text. Let's compare the previous and the new one
      // to check if they are actually different.
      answer_r a = ea->get_answer();
      question_r q = a->get_question();
      question_with_body* qwb = dynamic_cast<question_with_body*>(&q.get());
      // It has to be one to be impacted.
      HX2A_ASSERT(qwb);
      question_body_r qb = qwb->get_body();
      question_localization_body_r qlb = a->get_question_localization_body();
      string ptxt = qb->calculate_text(q->get_label(), pts, lang, qlb->get_text());
      // With new stack.
      string ntxt = qb->calculate_text(q->get_label(), nts, lang, qlb->get_text());
      
      if (ptxt != ntxt){
	return true;
      }
      
      pts.replace_answer(a);
      nts.replace_answer(a);
      return false;
    }
    
    entry_begin_loop* ebl = dynamic_cast<entry_begin_loop*>(&e.get());
    // Otherwise it cannot be impacted.
    HX2A_ASSERT(ebl);
    question_begin_loop_r qbl = ebl->get_question_begin_loop();
    json::value plov = compute_loop_operand(pts, lang, qbl, ploa);
    json::value nlov = compute_loop_operand(nts, lang, qbl, nloa);
    
    if (plov != nlov){
      // The loop operand changed.
      // It's a bit brutal but we're going to restart at the begin all the loop. Being finer would be super complex.
      return true;
    }
    
    pts.process_begin_loop(lang, qbl);
    nts.process_begin_loop(lang, qbl);
    return false;
  }

  // The iterator points at the answer to be replaced, and the new answer is given in argument.
  localized_question_p interview::revise_answer(history_type::iterator pos, const answer_r& na){
    // We must obtain the previous answer. At this occasion, we check that the iterator points at a "real" answer, not a begin or end loop.
    HX2A_ASSERT(pos != _history.cend());
    HX2A_ASSERT(*pos);
    entry_r e = **pos;
    entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());

    if (!ea){
      HX2A_LOG(trace) << "revise_answer: While revising an answer, history iterator does not point at an answer.";
      throw internal_error();
    }

    answer_r pa = ea->get_answer();
    question_r q = pa->get_question();
    
    // We must now validate the answer and check that it is an answer to the same question as the previous one.
    if (q != na->get_question()){
      throw answer_is_incorrect();
    }
    
    // Building the stack up to the replaced answer, but not including it.
    the_stack pts;
    calculate(pts, pos);
    // As we are going to compare the result of calculated texts and loop operands applying the "old" stack and the new one with the replaced answer, we
    // must make a copy.
    the_stack nts(pts);
    // We add the previous answer to the previous stack.
    pts.replace_answer(pa);
    // We add the new answer to the new stack.
    nts.replace_answer(na);
    // We can now safely replace the previous answer. We use graft rather than erase and insert, because erase triggers referential integrity and
    // will remove history begin loop entries that have the previous answer as a loop operand, which would be wrong.
    HX2A_LOG(trace) << "revise_answer: Grafting the new answer and moving ahead to the next entry in the history.";
    pa->graft(na);
    auto i = ++pos;
    auto he = _history.end();

    while (true){
      HX2A_LOG(trace) << "Running the new transitions on question " << q->get_label();
      // Let's run the new transitions.
      question_r nnetq = run_transitions(nts, q);
      
      // We might be at the end of the interview.
      if (i == he){
	HX2A_LOG(trace) << "revise_answer: We've reached the end of the history.";
	// We need to find the next regular question. It's either the one we have or a subsequent one.
	set_next_question(find_next_regular_question(nts, _language, nnetq, _start_timestamp));
	return next_localized_question(nts);
      }
      
      HX2A_LOG(trace) << "revise_answer: We've not reached the end of the history. The next question is " << nnetq->get_label();
      // Let's see what the next question was.
      HX2A_ASSERT(*i);
      // We have the next entry.
      entry_r nee = **i;
      // Let's see the associated question.
      question_r pnetq = nee->get_question();
      HX2A_LOG(trace) << "revise_answer: The previous next question is " << pnetq->get_label() << " while the new one is " << nnetq->get_label();
      
      // Let's compare.
      if (nnetq != pnetq){
	HX2A_LOG(trace) << "revise_answer: The next question is different.";
	// They are different. The answer change has driven to another question.
	// That other question might already have an answer subsequently in the history. If it does, we need to resect the history.
	resect(i, nnetq);
	
	if (i == he){
	  HX2A_LOG(trace) << "revise_answer: We've reached the end of the history.";
	  // We have removed everything, there was no answer to the question.
	  // We need to find the next regular question. It's either the one we have or a subsequent one.
	  set_next_question(find_next_regular_question(nts, _language, nnetq, _start_timestamp));
	  return next_localized_question(nts);
	}

	// We might have resected entries for answers some begin loops operate upon. So the logic that follows is hiding some issues.
	// These issues are structural to the questionnaire, which is ill-formed. We're not going to diagnose that. Even if subsequent
	// entries are impacted by the resections, we'll ignore them, and we will retain the answers. It is too late.
	// Diagnosing it is not impossible, though, we would need to keep track of all the resected answers and instead of checking
	// if there is an impact on the answer which is getting replaced, we would check if there is an impact from the resected
	// answers too. Too expensive for too little benefit (diagnosing an ill-formed questionnaire).
	
	// We have found an answer to the new next question.
	// The entry might be impacted because it is a begin loop or if it contains an answer to a question with a calculated text.
	// If it is a begin loop, we must calculate the previous operand array and the new one and compare to see if there is a
	// difference.
	// If the new operand array has commonality with the old one, we can preserve the identical loop iterations, if any.
	// If the question contains a calculated text, we must recalculate the old text and the new, compare, and if they are
	// different, we must truncate the history at this point and return. If they are identical, we can record the entry in
	// both stacks and keep going.

	HX2A_ASSERT(*i);
	nee = **i;

	if (nee->is_impacted_by(pa)){
	  // The function will return true if the entry is truly impacted.
	  // Otherwise it'll return false and will update the previous and new stacks.
	  if (process_impacted_entry(pts, nts, pa, na, _language, nee)){
	    // There was a true impact.
	    // woraround missing erase with [begin, end) iterators.
	    while (i != he){
	      history_type::iterator next = i;
	      ++next;
	      _history.erase(i);
	      i = next;
	    }
	    
	    // We need to find the next regular question. It's either the one we have or a subsequent one.
	    set_next_question(find_next_regular_question(nts, _language, nnetq, _start_timestamp));
	    return next_localized_question(nts);
	  }
	  else{
	    // It turns out there was no impact at all.
	    pts.process_entry(_language, nee);
	    nts.process_entry(_language, nee);
	    q = nnetq;
	    ++i;
	    continue;
	  }
	}
	else{
	  // Not impacted. We can keep going.
	  pts.process_entry(_language, nee);
	  nts.process_entry(_language, nee);
	  q = nnetq;
	  ++i;
	  continue;
	}
      }

      HX2A_LOG(trace) << "revise_answer: The next question is the same.";
      // The transition leads to the same question.
      // It might be impacted if it is a begin loop or if it contains a calculated text.
      // If not impacted, we need to add the entry to both stacks and keep going.
      
      if (nee->is_impacted_by(pa)){
	HX2A_LOG(trace) << "revise_answer: The next history entry is impacted.";
	
	// The function will return true if the entry is truly impacted.
	// Otherwise it'll return false and will update the previous and new stacks.
	if (process_impacted_entry(pts, nts, pa, na, _language, nee)){
	  // There was a true impact.
	  // woraround missing erase with [begin, end) iterators.
	  while (i != he){
	    history_type::iterator next = i;
	    ++next;
	    _history.erase(i);
	    i = next;
	  }
	    
	  // We need to find the next regular question. It's either the one we have or a subsequent one.
	  set_next_question(find_next_regular_question(nts, _language, nnetq, _start_timestamp));
	  return next_localized_question(nts);
	}
	else{
	  HX2A_LOG(trace) << "revise_answer: The next history entry is not impacted.";
	  // It turns out there was no impact at all.
	  pts.process_entry(_language, nee);
	  nts.process_entry(_language, nee);
	  q = nnetq;
	  ++i;
	  continue;
	}
      }

      HX2A_LOG(trace) << "revise_answer: The next history entry is not impacted.";
      
      // Not impacted. We need to keep going as subsequent entries might be impacted still.
      pts.process_entry(_language, nee);
      nts.process_entry(_language, nee);
      q = nnetq;
      ++i;
    } // End while (true).
  }

  // Returns the "true" answer with an index in the history which precedes the index given. Every answer has a distinct index, from 0 to the size
  // of the history minus 1. An index can correspond to a begin or end loop in the history.
  localized_answer_data_and_more_payload_p interview::get_previous_answer(size_t index){
    if (!index){
      // Requesting the last answer.
      // We must skip the begin and end loops until we find a "real" answer.
      size_t idx = _history.size() - 1;
      history_type::const_reverse_iterator i = _history.crbegin();
      history_type::const_reverse_iterator e = _history.crend();

      while (i != e){
	HX2A_ASSERT(*i);
	entry_r e = **i;

	if (e->get_loop_type() == question::regular){
	  the_stack ts;
	  // We have a reverse iterator, we convert it into a regular non-reverse iterator.
	  // We use a non STL-blessed ctor, we like it better that way.
	  history_type::const_iterator ci(i);
	  calculate(ts, ci);
	  ts.dump();
    
	  entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
	  HX2A_ASSERT(ea);
	  return make<localized_answer_data_and_more_payload>(ea->get_answer()->make_localized_answer_data(ts, _language), idx, idx != 0);
	}

	--idx;
	++i;
      }

      return {};
    }

    --index;
    // The call below updates the index to the first one found from the latest answers, which corresponds to a "real" answer and not a begin or end
    // loop.
    // It looks for the answer with an index inferior or equal to the index given.
    history_type::const_iterator i = find_answer_at_most(index);
    
    the_stack ts;
    calculate(ts, i);
    ts.dump();
    
    HX2A_ASSERT(*i);
    entry_r e = **i;
    HX2A_ASSERT(e->get_loop_type() == question::regular);
    entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
    HX2A_ASSERT(ea);
    return make<localized_answer_data_and_more_payload>(ea->get_answer()->make_localized_answer_data(ts, _language), index, i != _history.cbegin());
  }

  localized_answer_data_and_more_payload_p interview::get_next_answer(size_t index){
    ++index;
    history_type::const_iterator i = find_answer_at_least(index);
    
    the_stack ts;
    calculate(ts, i);
    ts.dump();
    
    HX2A_ASSERT(*i);
    entry_r e = **i;
    HX2A_ASSERT(e->get_loop_type() == question::regular);
    entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
    HX2A_ASSERT(ea);
    // Advancing the iterator to check if there is more.
    ++i;
    return make<localized_answer_data_and_more_payload>(ea->get_answer()->make_localized_answer_data(ts, _language), index, i != _history.cend());
  }

  // Resect is used when revising an answer. Because we do not want to ask the respondent to answer
  // unnecessarily questions they already answered, we rerun the transitions (which might contain non
  // repeatable - e.g., random - transitions), and retain everything already answered.
  // Answers which are not used any longer are simply dropped. We could improve the algorithm and put
  // answers not useful any longer is some stash, in case subsequent answer revisions use them. We stay
  // simple for now and do not go that far.
  void interview::resect(history_type::iterator& pos, const question_r& q){
    auto e = _history.end();

    while (pos != e){
      HX2A_ASSERT(*pos);
      entry_r e = **pos;

      if (e->get_question() == q){
	return;
      }

      // woraround missing returned next iterator in erase() function.
      history_type::iterator next = pos;
      ++next;
      _history.erase(pos);
      pos = next;
    }
  }

  // Gives the elapsed time since the last answer recorded in the interview, and the total elapsed time since beginning of the interview.
  // In case there is no answer yet, the timestamp of the start of the interview is used.
  pair<time_t, time_t> interview::calculate_elapsed_times() const {
    time_t now = time();
    HX2A_ASSERT(now >= _start_timestamp);
    time_t total_elapsed = now - _start_timestamp;
    time_t elapsed;

    if (_history.empty()){
      elapsed = total_elapsed;
    }
    else{
      answer_p if_a = last_answer();
      HX2A_ASSERT(if_a);
      answer_r a = *if_a;
      HX2A_ASSERT(now >= a->get_timestamp(_start_timestamp));
      elapsed = now - a->get_timestamp(_start_timestamp);
    }

    return {elapsed, total_elapsed};
  }

  static bool initialize_body(){
    // The result is most likely JavaScript's undefined value. It won't parse into a json::value. But it'll execute and put the functions
    // in the heap for subsequent reuse.
    try {
      v8::execute(javascript_library);
    }
    catch(const not_a_json_value&){}
    
    return true;
  }
  
  bool initialize(){
    [[maybe_unused]] static bool called = initialize_body();
    return true;
  }

  // Methodology to add a new question type
  //
  // All virtuals to be defined on each type.
  //
  // Ontology types:
  //   - If the new question can be a template, add a question body type in the ontology, wherever it fits best in the inheritance
  //     hierarchy. If it cannot be a template, define a new question type.
  //   - For a new question type, adjust virtual to taste (whether the question can be a final one if it has no transitions - by default it
  //     cannot, whether the new question type supports localization, by default it does not).
  
  //   - Add a question_localization_body in the ontology.
  //     The objects will be stored in questionnaires localizations.
  //   - Add an answer_body.
  //     The objects will be stored in interviews.
  // Payload types:
  //   - Add a source_question_body.
  //     This will be used to specify a question or a template question.
  //   - Find a new question type keyword, and add a mapping from it to the new source_question_body type meta in the source question types map.
  //   - Add a source_question_localization_body.
  //     This will be used to specify a localization for a question or a template question.
  //   - Add an answer_payload_body.
  //     This will be used to give an answer to a question through a service call.
  //   - Add a localized_answer_data_payload_body.
  //     This will contain both question and answer for a given language, to consult them from a service call.

} // End namespace interviews.
