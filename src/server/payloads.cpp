//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#include <unordered_map>
#include <utility>
#include <cctype>
#include <stack>

#include "hx2a/checked_cast.hpp"

#include "interviews/exception.hpp"
#include "interviews/payloads.hpp"

namespace interviews {

  using namespace hx2a;

  void source_function::compile(const string& label, const question_body_r& qb, const question_infos_by_label_map& qblm, const question_info& qi) const {
    if (_code.get().empty()){
      throw function_has_no_code(label);
    }
    
    function_r func = make<function>(_code);
    size_t qn = qi.get_index();

    for (const auto& par: _parameters){
      auto f = qblm.find(par);

      if (f == qblm.cend()){
	throw function_parameter_does_not_exist(label);
      }

      question_info pqi = f->second.first;
      size_t pqn = pqi.get_index();

      if (qn == pqn){
	throw function_parameter_refers_to_self(label);
      }

      if (qn < pqn){
	throw function_parameter_refers_to_subsequent_question(label);
      }

      // Comparing two vectors.
      if (qi.get_loop_nest() != pqi.get_loop_nest()){
	throw function_parameter_refers_to_question_with_different_loop_nest(label);
      }

      func->push_parameter_back(*f->second.second);
    }
    
    qb->push_text_function_back(func);
  }

  // Checks that if functions are used, their index does not reach fn or more.
  // Searches for pattersn like @{n} where n should be between 0 and fn - 1.
  // It does not validate loop variable names. They are validated at runtime.
  static inline void validate_parametric_text(const string& label, const string& text, size_t fn){
    auto i = text.cbegin();
    auto e = text.cend();

    while (i != e){
      if (*i == eval_prefix){
	++i;

	if (i == e){
	  return;
	}

	if (*i == eval_open){
	  // Read @{, need to accumulate the function number.
	  ++i;

	  if (i == e){
	    return;
	  }

	  char c = *i;

	  if (isdigit(c)){
	    uint64_t funcn = c - '0';
	    
	    do {
	      ++i;
	      
	      if (i == e){
		return;
	      }
	      
	      c = *i;

	      if (!isdigit(c)){
		break;
	      }
	      
	      // We let funny things happen in case of overflow. We do not check.
	      funcn += funcn * 10 + c - '0';
	    } while (true);

	    if (c == eval_close){
	      if (funcn >= fn){
		HX2A_LOG(trace) << "In question with label \"" << label << "\", found a parametric text with call to function of index " << funcn << ", while the number of functions is " << fn << '.';
		throw function_call_out_of_bounds(label);
	      }
	    }
	  } // End if (isdigit(c)).
	  // If it's not a digit, we don't care, we don't validate loop variable names.
	}
      }
      
      ++i;
    }
  }
  
  void source_text::compile(const string& label, const question_body_r& qb, const question_infos_by_label_map& qblm, const question_info& qi) const {
    for (const auto& f: _functions){
      if (!f){
	throw function_is_null(label);
      }
      
      f->compile(label, qb, qblm, qi);
    }

    // We need to check that all the functions used in the text exist.
    validate_parametric_text(label, _value, _functions.size());
  }
  
  // We are "flattening" everything as payloads do not have bodies like types in the ontology do.
  source_template_question::source_template_question(const template_question_localization_r& tql):
    _language(*this, tql->get_language()),
    _label(*this),
    _style(*this),
    _text(*this)
  {
    template_question_r tq = tql->get_template_question();
    _label = tq->get_label();
    question_body_r qb = tq->get_body();
    _style = qb->get_style();
    question_localization_body_r tqlb = tql->get_body();
    _text = tqlb->get_text();
  }

  // Must create both the template question AND the template question localization, as both are in
  // the source.
  template_question_r source_template_question_message::compile(const template_question_category_r& tqc) const {
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, make<question_body_message>(_style));
    make<template_question_localization>(*tqc->get_home(), tq, _language, make<question_localization_body_message>(_text));
    return tq;
  }

  void source_template_question_message::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    body->update(_style);
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    qlb->update(_text);
  }
  
  template_question_r source_template_question_input::compile(const template_question_category_r& tqc) const {
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, make<question_body_input>(_style, _comment_label.get().size(), _optional));
    make<template_question_localization>(*tqc->get_home(), tq, _language, make<question_localization_body_input>(_text, _comment_label));
    return tq;
  }

  void source_template_question_input::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbi = checked_cast<question_body_input>(body);
    qbi->update(_style, _comment_label.get().size());
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbi = checked_cast<question_localization_body_input>(qlb);
    qlbi->update(_text, _comment_label);
  }
  
  template_question_r source_template_question_select::compile(const template_question_category_r& tqc) const {
    rfr<question_body_with_options> qbwo = make<question_body_select>(_style, _randomize, _comment_label.get().size());
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
    rfr<question_localization_body_with_options> qlbwo = make<question_localization_body_select>(_text, _comment_label);
    make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
    // Now let's take care of the options for both in a single shot.
    compile_options(qbwo, qlbwo);
    return tq;
  }

  void source_template_question_select::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbs = checked_cast<question_body_select>(body);
    qbs->update(_style, _comment_label.get().size());
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbs = checked_cast<question_localization_body_select>(qlb);
    qlbs->update(_text, _comment_label);

    // Must now take care of the options on both the question and the localization.
    compile_options(qbs, qlbs);
  }
  
  template_question_r source_template_question_select_at_most::compile(const template_question_category_r& tqc) const {
    rfr<question_body_with_options> qbwo = make<question_body_select_at_most>(_style, _randomize, _comment_label.get().size(), _limit);
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
    rfr<question_localization_body_with_options> qlbwo = make<question_localization_body_select_at_most>(_text, _comment_label);
    make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
    // Now let's take care of the options for both in a single shot.
    compile_options(qbwo, qlbwo);
    return tq;
  }

  void source_template_question_select_at_most::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbs = checked_cast<question_body_select_at_most>(body);
    qbs->update(_style, _comment_label.get().size(), _limit);
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbs = checked_cast<question_localization_body_select_at_most>(qlb);
    qlbs->update(_text, _comment_label);

    // Must now take care of the options on both the question and the localization.
    compile_options(qbs, qlbs);
  }
  
  template_question_r source_template_question_select_limit::compile(const template_question_category_r& tqc) const {
    rfr<question_body_with_options> qbwo = make<question_body_select_limit>(_style, _randomize, _comment_label.get().size(), _limit);
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
    rfr<question_localization_body_with_options> qlbwo = make<question_localization_body_select_limit>(_text, _comment_label);
    make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
    // Now let's take care of the options for both in a single shot.
    compile_options(qbwo, qlbwo);
    return tq;
  }

  void source_template_question_select_limit::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbs = checked_cast<question_body_select_limit>(body);
    qbs->update(_style, _comment_label.get().size(), _limit);
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbs = checked_cast<question_localization_body_select_limit>(qlb);
    qlbs->update(_text, _comment_label);

    // Must now take care of the options on both the question and the localization.
    compile_options(qbs, qlbs);
  }
  
  template_question_r source_template_question_rank_at_most::compile(const template_question_category_r& tqc) const {
    rfr<question_body_with_options> qbwo = make<question_body_rank_at_most>(_style, _randomize, _comment_label.get().size(), _limit);
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
    rfr<question_localization_body_with_options> qlbwo = make<question_localization_body_rank_at_most>(_text, _comment_label);
    make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
    // Now let's take care of the options for both in a single shot.
    compile_options(qbwo, qlbwo);
    return tq;
  }

  void source_template_question_rank_at_most::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbs = checked_cast<question_body_rank_at_most>(body);
    qbs->update(_style, _comment_label.get().size(), _limit);
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbs = checked_cast<question_localization_body_rank_at_most>(qlb);
    qlbs->update(_text, _comment_label);

    // Must now take care of the options on both the question and the localization.
    compile_options(qbs, qlbs);
  }
  
  template_question_r source_template_question_rank_limit::compile(const template_question_category_r& tqc) const {
    rfr<question_body_with_options> qbwo = make<question_body_rank_limit>(_style, _randomize, _comment_label.get().size(), _limit);
    template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
    rfr<question_localization_body_with_options> qlbwo = make<question_localization_body_rank_limit>(_text, _comment_label);
    make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
    // Now let's take care of the options for both in a single shot.
    compile_options(qbwo, qlbwo);
    return tq;
  }

  void source_template_question_rank_limit::update(const template_question_localization_r& tql) const {
    // Updating the template question first.
    template_question_r tq = tql->get_template_question();
    tq->update(_label);
    question_body_r body = tq->get_body();
    auto qbs = checked_cast<question_body_rank_limit>(body);
    qbs->update(_style, _comment_label.get().size(), _limit);
    
    // Updating the template question localization now.
    // Processing the new language if any.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    auto qlbs = checked_cast<question_localization_body_rank_limit>(qlb);
    qlbs->update(_text, _comment_label);

    // Must now take care of the options on both the question and the localization.
    compile_options(qbs, qlbs);
  }

  void template_question_localization_update_payload::update() const {
    db::connector cn{dbname};
    
    template_question_localization_r tql = template_question_localization::get(cn, _template_question_localization_id).or_throw<template_question_localization_does_not_exist>();
    
    if (_language.get() == language::nil()){
      throw template_question_language_is_invalid();
    }

    // It'll refuse if the new language is already supported.
    tql->update_language(_language);
    question_localization_body_r qlb = tql->get_body();
    qlb->update(_text);
    update_supplemental(qlb, tql->get_template_question());
  }

  // Specializations for update_supplemental.
  
  void template_question_localization_with_comment_update_payload::update_supplemental(const question_localization_body_r& qlb, const template_question_r&) const {
    auto qlbwc = checked_cast<question_localization_body_with_comment>(qlb);
    qlbwc->set_comment_label(_comment_label);
  }
  
  void template_question_localization_with_options_update_payload::update_supplemental(const question_localization_body_r& qlb, const template_question_r& tq) const {
    template_question_localization_with_comment_update_payload::update_supplemental(qlb, tq);
    auto qbwo = checked_cast<question_body_with_options>(tq->get_body());
    
    if (_options.size() != qbwo->get_options_size()){
      throw template_question_localization_options_size_is_incorrect();
    }
    
    auto i = qbwo->options_cbegin();
    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    qlbwo->options_clear();
    
    for (const auto& sch: _options){
      if (!sch){
	throw source_question_contains_null_option(tq->get_label());
      }
      
      HX2A_ASSERT(*i);
      qlbwo->push_option_back(make<option_localization>(tq->get_label(), **i, sch->_label, sch->_comment_label));
      ++i;
    }
  }
  
  // End of specializations for update_supplemental.
  
  template_question_localization_r source_template_question_localization::compile() const {
    db::connector cn{dbname};
    
    // Let's fetch the template_question.
    template_question_p tq = template_question::get(cn, _template_question_id);
    
    if (!tq){
      throw template_question_does_not_exist();
    }
    
    // We don't care if language is null. It won't find anything.
    if (template_question_localization::find(*tq, _language)){
      throw template_question_localization_already_exists();
    }
    
    return compile_supplemental(*cn, *tq, _language);
  }

  // Specializations for compile_supplemental.

  template_question_localization_r source_template_question_localization_message::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    return make<template_question_localization>(c, tq, lang, make<question_localization_body_message>(_text));
  }
  
  template_question_localization_r source_template_question_localization_input::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    return make<template_question_localization>(c, tq, lang, make<question_localization_body_input>(_text, _comment_label));
  }
  
  template_question_localization_r source_template_question_localization_select::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    rfr<question_localization_body_select> qlb = make<question_localization_body_select>(_text, _comment_label);
    compile_options(qlb, tq);
    return make<template_question_localization>(c, tq, lang, qlb);
  }
  
  template_question_localization_r source_template_question_localization_select_at_most::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    rfr<question_localization_body_select_at_most> qlb = make<question_localization_body_select_at_most>(_text, _comment_label);
    compile_options(qlb, tq);
    return make<template_question_localization>(c, tq, lang, qlb);
  }
  
  template_question_localization_r source_template_question_localization_select_limit::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    rfr<question_localization_body_select_limit> qlb = make<question_localization_body_select_limit>(_text, _comment_label);
    compile_options(qlb, tq);
    return make<template_question_localization>(c, tq, lang, qlb);
  }
  
  template_question_localization_r source_template_question_localization_rank_at_most::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    rfr<question_localization_body_rank_at_most> qlb = make<question_localization_body_rank_at_most>(_text, _comment_label);
    compile_options(qlb, tq);
    return make<template_question_localization>(c, tq, lang, qlb);
  }
  
  template_question_localization_r source_template_question_localization_rank_limit::compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
    rfr<question_localization_body_rank_limit> qlb = make<question_localization_body_rank_limit>(_text, _comment_label);
    compile_options(qlb, tq);
    return make<template_question_localization>(c, tq, lang, qlb);
  }
  
  // End of specializations for compile_supplemental.
  
  std::pair<question_r, question_localization_p> source_question_message::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    // A message without text and just a comment label is acceptable. E.g. "If you have a general feedback for this questionnaire, please enter it below."
    if (_text.get() == nullptr || _text->_value.get().empty()){
      // There needs to be a text.
      throw source_question_text_is_missing(_label);
    }
    
    question_r q = make<question_with_body>(_label, make<question_body_message>(_style));
    // We must push the question in the questionnaire so that referential integrity is happy that the localization we're going to build bears
    // a link to it.
    qq->push_question_back(q);
    return {q, make<question_localization>(q, make<question_localization_body_message>(_text->_value))};
  }

  std::pair<question_r, question_localization_p> source_question_input::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    if (_text.get() == nullptr || _text->_value.get().empty()){
      // There needs to be a text. It is the label of the input field.
      throw source_question_text_is_missing(_label);
    }

    question_r q = make<question_with_body>(_label, make<question_body_input>(_style, _comment_label.get().size(), _optional));
    // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
    // a link to it.
    qq->push_question_back(q);
    return {q, make<question_localization>(q, make<question_localization_body_input>(_text->_value, _comment_label))};
  }
    
  std::pair<question_r, question_localization_p> source_question_select::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    if (_text.get() == nullptr || _text->_value.get().empty()){
      // There needs to be a text. It is the label of the input field.
      throw source_question_text_is_missing(_label);
    }

    if (_options.size() <= 1){
      throw source_question_has_invalid_options(_label);
    }
	
    question_body_with_options_r cb = make<question_body_select>(_style, _randomize, _comment_label.get().size());
    question_r q = make<question_with_body>(_label, cb);
    // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
    // a link to it.
    qq->push_question_back(q);
    question_localization_body_select_r qlb = make<question_localization_body_select>(_text->_value, _comment_label);
    question_localization_r ql = make<question_localization>(q, qlb);
    compile_options(cb, qlb);
    return {q, ql};
  }
    
  std::pair<question_r, question_localization_p> source_question_select_at_most::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    return tmpl_compile<question_body_select_at_most, question_localization_body_select_at_most>(qq);
  }
    
  std::pair<question_r, question_localization_p> source_question_select_limit::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    return tmpl_compile<question_body_select_limit, question_localization_body_select_limit>(qq);
  }
    
  std::pair<question_r, question_localization_p> source_question_rank_at_most::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    return tmpl_compile<question_body_rank_at_most, question_localization_body_rank_at_most>(qq);
  }
    
  std::pair<question_r, question_localization_p> source_question_rank_limit::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    return tmpl_compile<question_body_rank_limit, question_localization_body_rank_limit>(qq);
  }
    
  std::pair<question_r, question_localization_p> source_question_from_template::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    template_question_p tq = template_question::find(*qq->get_home(), _template_name);

    if (!tq){
      throw template_question_does_not_exist();
    }
    
    question_r q = make<question_from_template>(_label, *tq);
    // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
    // a link to it.
    qq->push_question_back(q);
    // A question from template is not allowed to have a localization.
    return {q, {}};
  }

  // We could check that there is no variable name collision in nested loops... Oh well...
  std::pair<question_r, question_localization_p> source_question_begin_loop::compile(const questionnaire_r& qq, const question_infos_by_label_map& qbl) const {
    auto f = qbl.find(_question);

    if (f == qbl.cend()){
      throw question_begin_loop_refers_to_unknown_question(_label);
    }

    HX2A_ASSERT(f->second.second);
    // It'll check that the operand and the variable are not empty.
    question_r q = make<question_begin_loop>(_label, *f->second.second, _variable, _operand);
    // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
    // a link to it.
    qq->push_question_back(q);
    return {q, {}};
  }
    
  std::pair<question_r, question_localization_p> source_question_end_loop::compile(const questionnaire_r& qq, const question_infos_by_label_map&) const {
    question_r q = make<question_end_loop>(_label);
    // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
    // a link to it.
    qq->push_question_back(q);
    return {q, {}};
  }
    
  source_question::source_question(const question_localization_r& ql):
    _label(*this, ql->get_question()->get_label()),
    _transitions(*this)
  {
    // Must take care of the transitions.
    // They are in the same order in the source and in the questionnaire. No search needed.
    {
      question_r q = ql->get_question();
      auto i = q->transitions_cbegin();
      auto e = q->transitions_cend();

      while (i != e){
	transition_p t = *i;
	HX2A_ASSERT(t);
	_transitions.push_back(make<source_transition>(*t));
	++i;
      }
    }
  }

  void source_question_inline::compile_more(const question_infos_by_label_map& qblm, const question_info& qi, questionnaire::questions_type::const_iterator i, questionnaire::questions_type::const_iterator e) const {
    HX2A_ASSERT(*i);
    question_body_r qb = (*i)->get_body();
    HX2A_ASSERT(_text);
    _text->compile(_label, qb, qblm, qi);
    compile_even_more(qblm, qi, i, e);
  }

  source_questionnaire::source_questionnaire(const questionnaire_r& qq, const questionnaire_localization_r& qql):
    header_payload(qq->get_logo(), qql->get_title()),
    _code(*this, qq->get_code()),
    _name(*this, qq->get_name()),
    _language(*this, qql->get_language()),
    _questions(*this)
  {
    qql->check();
    questionnaire_localization_map_per_question m;
    qql->dump(m);
    auto me = m.cend();
    auto i = qq->questions_cbegin();
    auto e = qq->questions_cend();

    while (i != e){
      question_p if_q = *i;
      HX2A_ASSERT(if_q);
      question_r q = *if_q;
      // We look for the localization first among the questionnaire localizations, even in the case the question might
      // be a template question. This is because questionnaire localizations trump template question localizations.
      auto qli = m.find(&q.get());

      if (qli == me){
	// Can't find the question localization in the questionnaire localization. It means that the question does not support localization.
	_questions.push_back(q->make_source_question(qql->get_language()));
      }
      else{
	HX2A_ASSERT(qli->second);
	_questions.push_back((*qli->second)->make_source_question());
      }
      
      ++i;
    }
  }

  void source_question::compile_transitions(const question_infos_by_label_map& m, const question_info& qqi, questionnaire::questions_type::const_iterator qi, questionnaire::questions_type::const_iterator qe) const {
    HX2A_ASSERT(*qi);
    question_r q = **qi;
    
    // Zero transition is allowed. It means it's either a final question (there can be several ones), or it is transitioning
    // to the immediate next question unconditionally.
    // For instance a message question cannot have zero transitions in the source if it is followed by another question and is
    // expected to be final.
    if (_transitions.empty()){
      if (q->can_be_final()){
	return;
      }
      
      // Transitioning to the immediate next question (if any) unconditionally.
      if (qi == qe){
	// No more question, error!
	throw source_question_transition_is_missing(_label);
      }

      ++qi;
      q->push_transition_back(make<transition>(**qi));
      return;
    }

    // There are transitions. Let's check that the last one is a catch-all.
    source_transition_r t = (_transitions.cend() - 1)->or_throw<source_question_contains_null_transition>(_label);

    if (!t->_condition.get().empty()){
      throw source_question_transitions_lack_catch_all(_label, t->_destination);
    }

    auto i = _transitions.cbegin();
    auto e = _transitions.cend();
    auto l = e - 1; // The last one.
    size_t qn = qqi.get_index();
    question_begin_loop_p qpbl = qqi.get_parent_begin_loop();

    // We could also check that different transitions always have different destinations.
    while (i != e){
      t = i->or_throw<source_question_contains_null_transition>(_label);

      if (i != l){
	if (t->_condition.get().empty() && t->_code.get().empty()){
	  // Cannot have a catch-all before the last one.
	  throw source_question_transition_catch_all_is_not_last(_label, t->_destination);
	}
      }

      // Let's check that the destination label exists.
      auto f = m.find(t->_destination);

      if (f == m.cend()){
	throw source_question_transition_does_not_exist(_label, t->_destination);
      }

      // Let's check that the transition is ahead.
      question_info dqi = f->second.first;
      size_t dn = dqi.get_index(); // Destination question number.
      
      if (qn == dn){
	throw source_question_transitions_to_itself(_label);
      }

      if (qn > dn){
	throw source_question_transitions_to_previous_question(_label, t->_destination);
      }

      // Let's check that the transition is not across a loop.
      // We have already checked that transitions are always ahead, so we assume below that we cannot
      // face a transition prior to the question.
      question* dq = f->second.second;

      switch (q->get_loop_type()){
	// Regular questions can only transition to questions with the same parent begin loop, as long
	// as they are not an end loop with a parent begin loop not matching their parent begin loop.
	// Comments:
	// If a question has a parent begin loop, it can transfer to the end loop matching that parent
	// begin loop. As that end loop has the same parent begin loop, the test below covers that
	// case too.
      case question::regular:
	// End loop transitions are run when the loop is exhausted.
	// Same rule as regular questions.
      case question::end_loop:
	{
	  question_begin_loop_p dqpbl = dqi.get_parent_begin_loop();
	  
	  if (
	      qpbl != dqpbl ||
	      (dq->get_loop_type() == question::end_loop && qpbl != dqi.get_matching_begin_loop())
	      ){
	    throw source_question_transitions_across_loop(q->get_label(), dq->get_label());
	  }
	  
	  break;
	}

      case question::begin_loop:
	{
	  switch (dq->get_loop_type()){
	    // Begin loops cannot transition to another begin loop, as that destination begin loop needs
	    // to iterate over the answer to a question of the same parent begin loop.
	  case question::begin_loop:
	    {
	      throw source_question_begin_loop_transitions_to_begin_loop(q->get_label(), dq->get_label());
	    }

	    // Begin loops can transition to an end loop only if that end loop is the end matching the
	    // begin loop.
	  case question::end_loop:
	    {
	      if (dqi.get_matching_begin_loop() != q){
		throw source_question_transitions_across_loop(q->get_label(), dq->get_label());
	      }

	      break;
	  }
	    
	    // Begin loops can only transition to a regular question if the regular question ahs the begin
	    // loop as a parent begin loop.
	  default:
	    {
	      question_begin_loop_p dqpbl = dqi.get_parent_begin_loop();
	      
	      if (dqpbl != q){
		throw source_question_transitions_across_loop(q->get_label(), dq->get_label());
	      }
	      
	      break;
	    }
	  }
	}
	
	break;
      }
      
      // All is good, we can add the transition.
      // Let's see if a condition or code has been given.
      string ocode;
      const string& condition = t->_condition;

      if (condition.size()){
	// The condition was specified. Let's check that code has not been specified too.
	if (t->_code.get().size()){
	  // It was, we're confused.
	  throw transition_has_both_condition_and_code(_label);
	}

	// We're good, it's only a condition.
	ocode = condition;
      }
      else{
	const string& code = t->_code;

	if (code.size()){
	  ocode = code;
	}
      }

      function_r func = make<function>(ocode);
      transition_r ot = make<transition>(func, *f->second.second);
      
      // Let's process the arguments.
      for (const auto& par: t->_parameters){
	// Let's check that the argument corresponds to an existing question.
	auto f = m.find(par);

	if (f == m.cend()){
	  throw source_question_argument_does_not_exist(_label, par);
	}

	ot->push_parameter_back(*f->second.second);
      }
      
      q->push_transition_back(ot);

      // Let's check the transitions.
      try{
	q->check_conditions();
      }
      catch(...){
	throw source_question_transition_condition_is_incorrect(_label, t->_destination);
      }
      
      ++i;
    }
  }
  
  // Because of referential integrity constraints, compilation is performed in two passes, first all the questions
  // are created, then transitions are validated and assigned.
  std::pair<questionnaire_r, questionnaire_localization_r> source_questionnaire::compile(const db::connector& c) const {
    // The code can be empty.
    
    if (_name.get().empty()){
      throw source_questionnaire_name_is_empty();
    }

    if (_questions.empty()){
      throw source_questionnaire_has_no_questions();
    }

    questionnaire_r qq = make<questionnaire>(c, _code, _name, _logo);
    // This will check again that the comment label presence is correct. A little imperfection.
    questionnaire_localization_r ql = make<questionnaire_localization>(c, qq, _title, _language, _name);

    // Let's keep a map of questions by label, it'll be used in the second pass.
    question_infos_by_label_map m;
    
    // First pass, creating all the questions, including their options and their localizations.
    size_t qn = 0; // The first question is question number 0.

    loop_nest ln;
    
    for (const auto& sq: _questions){
      if (!sq){
	throw source_questionnaire_contains_null_question();
      }

      HX2A_LOG(trace) << "Compiling source question with label " << sq->_label;
      
      // This will add the newly created question to the questionnaire.
      // The label is validated by the question's ctor.
      std::pair<question_r, question_localization_p> qql = sq->compile(qq, m);
      question_r q = qql.first;
      
      if (m.find(q->get_label()) != m.cend()){
	throw question_label_is_a_duplicate(q->get_label());
      }

      if (ln.empty()){
	m.emplace(q->get_label(), pair<question_info, question*>(question_info(qn, {}, {}), &q.get()));
	// Will throw if it is an end loop.
	q->update_loop_nest(ln);
      }
      else if (q->get_loop_type() == question::end_loop){
	question_begin_loop_r mbl = ln.back();
	ln.pop_back(); // We know how to update the loop nest.
	m.emplace(q->get_label(), pair<question_info, question*>(question_info(qn, ln, mbl), &q.get()));
	HX2A_LOG(trace) << "It is an end loop, its level is " << ln.size() << ", its matching begin loop has label " << mbl->get_label() << ", while its parent begin loop is " << (ln.size() ? ln.back()->get_label() : "null");
      }
      else{
	m.emplace(q->get_label(), pair<question_info, question*>(question_info(qn, ln, {}), &q.get()));
	q->update_loop_nest(ln);
      }

      if (qql.second){
	ql->push_question_localization_back(*qql.second);
      }
      
      ++qn;
    }

    if (ln.size()){
      throw question_loop_is_not_closed(ln.back()->get_label());
    }

    // We must check that begin loops iterate over answers corresponding to a question at the same level as the begin loop.
    // Quite expensive, but needed.
    {
      leveled_questionnaire lq;
      qq->dump(lq);
      // We could have kept only the begin loops before, and iterate only upon them.
      auto i = qq->questions_cbegin();
      auto e = qq->questions_cend();

      while (i != e){
	HX2A_ASSERT(*i);
	question_r q = **i;

	if (q->get_loop_type() == question::begin_loop){
	  question_begin_loop* qbl = dynamic_cast<question_begin_loop*>(&q.get());
	  HX2A_ASSERT(qbl);
	  question_r qbloq = qbl->get_operand_question();
	  auto f = lq.find(&q.get());
	  HX2A_ASSERT(f != lq.cend());
	  // Question information on the begin loop itself.
	  question_info qi = f->second;
	  f = lq.find(&qbloq.get());
	  HX2A_ASSERT(f != lq.cend());
	  // Question information on the question the answer to the begin loop iterates upon.
	  question_info qiq = f->second;

	  // Comparing two vectors.
	  if (qi.get_loop_nest() != qiq.get_loop_nest()){
	    throw question_begin_loop_refers_to_question_with_different_loop_nest(q->get_label());
	  }
	}
	
	++i;
      }
    }


    // Second pass, creating all the transitions.
    // We have exactly the same number of questions and in the same order in the questionnaire and in the source questionnaire.
    // We can iterate on both at the same time.
    questionnaire::questions_type::const_iterator qi = qq->questions_cbegin();
    questionnaire::questions_type::const_iterator qe = qq->questions_cend();
    
    for (const auto& sq: _questions){
      // We have already performed the check on sq not being null.
      HX2A_ASSERT(*qi);
      auto f = m.find((*qi)->get_label());
      HX2A_ASSERT(f != m.cend());
      // This will check that non final questions have at least one transition, and that a catch all transition
      // is present at the end for the non final questions.
      sq->compile(m, f->second.first, qi, qe);
      ++qi;
      ++qn;
    }

    return std::pair<questionnaire_r, questionnaire_localization_r>(qq, ql);
  }

  question_localization_r source_question_localization_message::compile(const question_r& q) const {
    question_localization_r ql = make<question_localization>(q, make<question_localization_body_message>(_text));
    ql->check(); 
    return ql;
  }
    
  question_localization_r source_question_localization_input::compile(const question_r& q) const {
    question_localization_r ql = make<question_localization>(q, make<question_localization_body_input>(_text, _comment_label));
    ql->check(); 
    return ql;
  }

  question_localization_r source_question_localization_select::compile(const question_r& q) const {
    return tmpl_compile<question_localization_body_select>(q);
  }
    
  question_localization_r source_question_localization_select_at_most::compile(const question_r& q) const {
    return tmpl_compile<question_localization_body_select_at_most>(q);
  }
    
  question_localization_r source_question_localization_select_limit::compile(const question_r& q) const {
    return tmpl_compile<question_localization_body_select_limit>(q);
  }
    
  question_localization_r source_question_localization_rank_at_most::compile(const question_r& q) const {
    return tmpl_compile<question_localization_body_rank_at_most>(q);
  }
    
  question_localization_r source_question_localization_rank_limit::compile(const question_r& q) const {
    return tmpl_compile<question_localization_body_rank_limit>(q);
  }
    
  question_localization_r source_question_localization::compile(const question_infos_by_label_map& map) const {
    auto i = map.find(_label);

    if (i == map.cend()){
      throw question_label_does_not_exist(_label);
    }

    // We don't need a smart pointer, the question is held in the questionnaire.
    question* q = i->second.second;
    HX2A_ASSERT(q);
    return compile(*q);
  }

  source_questionnaire_localization::source_questionnaire_localization(const questionnaire_localization_r& ql):
    _questionnaire_id(*this, ql->get_questionnaire()->get_id()),
    _title(*this, ql->get_title()),
    _language(*this, ql->get_language()),
    _questions(*this)
  {
    auto i = ql->questions_localizations_cbegin();
    auto e = ql->questions_localizations_cend();

    while (i != e){
      HX2A_ASSERT(*i);
      _questions.push_back(make<source_question_localization>(**i));
      ++i;
    }
  }
    
  questionnaire_localization_r source_questionnaire_localization::compile() const {
    db::connector cn{dbname};
    
    // Let's fetch the questionnaire.
    questionnaire_r q = questionnaire::get(cn, _questionnaire_id).or_throw<questionnaire_does_not_exist>();

    // It is not allowed to create a localization when the questionnaire is locked and a campaign has started. Indeed, if interviews are already using an old one,
    // they'll be removed. We could be smarter and check whether a previous localization exists and if it does if interviews already use it, but that would make
    // the code quite complicated and tedious to explain. The rule is therefore simple.
    if (q->is_locked()){
      throw questionnaire_is_locked();
    }
    
    question_infos_by_label_map m;
    q->dump(m);
    
    // We could check the language, there is no guarantee that the number sent is not in a hole or outside
    // of the valid numeric values. 
    questionnaire_localization_r ql = make<questionnaire_localization>(*cn, q, _title, _language, q->get_name());
    
    for (const auto& qu: _questions){
      if (!qu){
	throw source_questionnaire_contains_null_question();
      }
      
      ql->push_question_localization_back(qu->compile(m));
    }
    
    // Time to check. We have the check function on the questionnaire localization for other purposes, so
    // let's use it at this stage and not before.
    ql->check();
    return ql;
  }

  answer_body_r answer_message_payload::compile(const question_body_r&, const question_localization_body_r&) const {
    return make<answer_body_message>();
  }
  
  answer_body_r answer_input_payload::compile(const question_body_r&, const question_localization_body_r&) const {
    return make<answer_body_input>(_input, _comment);
  }
  
  answer_body_r answer_select_payload::compile(const question_body_r&, const question_localization_body_r& qlb) const {
    // The GUI should have already checked that. Belt and suspenders.
    if (!_choice){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }

    auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
    
    // We also check that the choice's index does not exceed the number of options.
    if (_choice->_index >= qlbwo->get_options_size()){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }

    return make<answer_body_select>(make<choice>(qlbwo->find_option_localization(_choice->_index), _choice->_index, _choice->_comment), _comment);
  }
  
  answer_body_r answer_select_at_most_payload::compile(const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qbst = checked_cast<question_body_select_at_most>(qb);
    
    if (_choices.size() > qbst->get_limit()){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }
    
    auto ab = make<answer_body_select_at_most>(_comment);
    // Let's process the choices.
    shared_compile(qlb, ab);
    return ab;
  }
  
  answer_body_r answer_select_limit_payload::compile(const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qbst = checked_cast<question_body_select_limit>(qb);
    
    if (_choices.size() != qbst->get_limit()){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }
    
    auto ab = make<answer_body_select_limit>(_comment);
    // Let's process the choices.
    shared_compile(qlb, ab);
    return ab;
  }
  
  answer_body_r answer_rank_at_most_payload::compile(const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qbst = checked_cast<question_body_rank_at_most>(qb);
    
    if (_choices.size() > qbst->get_limit()){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }
    
    auto ab = make<answer_body_rank_at_most>(_comment);
    // Let's process the choices.
    shared_compile(qlb, ab);
    return ab;
  }
  
  answer_body_r answer_rank_limit_payload::compile(const question_body_r& qb, const question_localization_body_r& qlb) const {
    auto qbst = checked_cast<question_body_rank_limit>(qb);
    
    if (_choices.size() != qbst->get_limit()){
      // The GUI failed to enforce the rules.
      throw answer_is_incorrect();
    }
    
    auto ab = make<answer_body_rank_limit>(_comment);
    // Let's process the choices.
    shared_compile(qlb, ab);
    return ab;
  }
  
  answer_r answer_payload::make_answer(const question_localization_r& ql, string_view ip_address, time_t elapsed, time_t total_elapsed) const {
    return make<answer>(ql, ip_address, elapsed, total_elapsed, _geo_location, compile(ql->get_question()->get_body(), ql->get_body()));
  }
  
  answer_r answer_payload::make_answer(const template_question_localization_r& tql, const question_from_template_r& qft, string_view ip_address, time_t elapsed, time_t total_elapsed) const {
    return make<answer>(tql, qft, ip_address, elapsed, total_elapsed, _geo_location, compile(tql->get_template_question()->get_body(), tql->get_body()));
  }
  
  interview_data::interview_data(const interview_r& i):
    _start_ip_address(*this, i->get_start_ip_address()),
    _start_timestamp(*this, i->get_start_timestamp()),
    _start_geolocation(*this), // Cannot transmit the start geolocation from the interview, it is already owned.
    _interviewee_id(*this, i->get_interviewee_id()),
    _interviewer_id(*this, i->get_interviewer_id()),
    _interviewer_user(*this),
    _language(*this, i->get_language()),
    _answers(*this),
    _state(*this, i->get_state())
  {
    user_p iu = i->get_interviewer_user();
    
    if (iu){
      _interviewer_user = make<user_data>(*iu);
    }
    
    auto hi = i->history_cbegin();
    auto he = i->history_cend();

    while (hi != he){
      HX2A_ASSERT(*hi);
      entry_r e = **hi;
      entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());

      // We do not include (yet) the begin and end loops.
      if (ea){
	_answers.push_back(ea->get_answer()->make_answer_data(_start_timestamp));
      }
      
      ++hi;
    }
    
    geolocation_p geo = i->get_start_geolocation();

    if (geo){
      // Can't cut it and paste it.
      _start_geolocation = (*geo)->copy();
    }
  }

  answer_data::answer_data(const answer_r& an, time_t start_timestamp):
    _label(*this, an->get_label()),
    _ip_address(*this, an->get_ip_address()),
    _timestamp(*this, an->get_timestamp(start_timestamp)),
    _elapsed(*this, an->get_elapsed()),
    _total_elapsed(*this, an->get_total_elapsed()),
    _geo_location(*this) // Cannot transmit the geolocation from the answer, it is already owned.
  {
    geolocation_p geo = an->get_geolocation();

    if (geo){
      // Can't cut it and paste it.
      _geo_location = (*geo)->copy();
    }
  }

  // In the language the interview was taken in.
  localized_interview_data::localized_interview_data(const interview_r& i):
    _interviewee_id(*this, i->get_interviewee_id()),
    _interviewer_id(*this, i->get_interviewer_id()),
    _interviewer_user(*this),
    _language(*this, i->get_language()),
    _answers(*this),
    _state(*this, i->get_state())
  {
    user_p iu = i->get_interviewer_user();
    
    if (iu){
      _interviewer_user = make<user_data>(*iu);
    }
    
    the_stack ts;
    i->calculate(ts);
    ts.dump();
    
    auto hi = i->history_cbegin();
    auto he = i->history_cend();

    while (hi != he){
      HX2A_ASSERT(*hi);
      entry_r e = **hi;
      entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());

      // We do not include (yet) the begin and end loops.
      if (ea){
	_answers.push_back(ea->get_answer()->make_localized_answer_data(ts, i->get_language()));
      }
      
      ++hi;
    }
  }
  
  // Possibly in another language the interview was taken in.
  localized_interview_data::localized_interview_data(const interview_r& i, language_t lang):
    _interviewee_id(*this, i->get_interviewee_id()),
    _interviewer_id(*this, i->get_interviewer_id()),
    _interviewer_user(*this),
    _language(*this, lang), // We get what we ask for, a little silly, the client does not need it. Anyway...
    _answers(*this),
    _state(*this, i->get_state())
  {
    user_p iu = i->get_interviewer_user();
    
    if (iu){
      _interviewer_user = make<user_data>(*iu);
    }
    
    the_stack ts;
    i->calculate(ts);
    ts.dump();
    
    auto hi = i->history_cbegin();
    auto he = i->history_cend();
    
    if (i->get_language() == lang){
      while (hi != he){
	HX2A_ASSERT(*hi);
	entry_r e = **hi;

	if (entry_answer* ea = dynamic_cast<entry_answer*>(&e.get())){
	  // We do not include (yet) the begin and end loops.
	  _answers.push_back(ea->get_answer()->make_localized_answer_data(ts, lang));
	}
	
	++hi;
      }

      return;
    }

    questionnaire_localization_p if_qql = i->get_questionnaire_localization();
    HX2A_ASSERT(if_qql);
    questionnaire_localization_r qql = *if_qql;

    // So it's really a different language. Must look for a localization, if any.
    questionnaire_localization_r ql = questionnaire_localization::find(qql->get_questionnaire(), lang).or_throw<questionnaire_localization_does_not_exist>();
    
    // Found it! Checking it. No worries the check returns immediately if already done.
    ql->check();

    questionnaire_localization_map_per_question m;
    ql->dump(m);
    auto me = m.cend();
    
    while (hi != he){
      HX2A_ASSERT(*hi);

      entry_r e = **hi;
      entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
      
      // We do not include (yet) the begin and end loops.
      if (ea){
	answer_r a = ea->get_answer();
	auto mi = m.find(&a->get_question().get());
	
	if (mi == me){
	  throw internal_error();
	}
	
	question_localization_p loc = mi->second;
	
	if (!loc){
	  // The questionnaire checked fine, we have an issue.
	  throw internal_error();
	}
	
	_answers.push_back(a->make_localized_answer_data(ts, lang, *loc));
      }
      
      ++hi;
    }
  }

} // End namespace interviews.
