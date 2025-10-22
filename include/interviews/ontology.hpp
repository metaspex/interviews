//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#ifndef HX2A_INTERVIEWS_ONTOLOGY_HPP
#define HX2A_INTERVIEWS_ONTOLOGY_HPP

// General design notes:
//
// This ontology is very polymorphic. Questionnaires and questionnaires localizations can contain a lot of types
// which are visible only as their parent type. Questions or question localizations have many different forms.
// Instead of multiplying all type hierarchies, some sharing is perform by using "bodies". Types that differ only
// in a limited manner (e.g., questions and template questions) bear a own relationship (called the "body") to
// the same polymorphic type. This sharing is justified only by type sharing, to avoid having a very complex
// ontology. The body does not have an independent life that would justify it being a separate object. The presence
// of a body actually slows down the execution. We accept that trade-off for ontology simplicity. There are
// maintenance advantages of that. Adding new question types has a smaller impact for instance.

#include <iterator>
#include <optional>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>
#include <variant>

#include "hx2a/root.hpp"
#include "hx2a/element.hpp"
#include "hx2a/anchor.hpp"
#include "hx2a/slot.hpp"
#include "hx2a/slot_js.hpp"
#include "hx2a/own.hpp"
#include "hx2a/own_list.hpp"
#include "hx2a/link.hpp"
#include "hx2a/weak_link.hpp"
#include "hx2a/link_list.hpp"
#include "hx2a/json_value.hpp"
#include "hx2a/detail/overloaded.hpp"
// For localization.
#include "hx2a/components/language.hpp"
// For location.
#include "hx2a/components/position.hpp"
#include "hx2a/components/user.hpp"
#include "hx2a/checked_cast.hpp"

#include "interviews/tags.hpp"
#include "interviews/exception.hpp"

namespace interviews {

  using std::pair;
  
  using namespace hx2a;

  // Basic type definitions.
  
  using percentage_t = unsigned int;
  using progress_t = percentage_t;
    
  // Payload types.

  class source_text;
  using source_text_p = ptr<source_text>;
  using source_text_r = rfr<source_text>;
  
  class source_option;
  using source_option_p = ptr<source_option>;
  using source_option_r = rfr<source_option>;
  
  class source_question;
  using source_question_p = ptr<source_question>;
  using source_question_r = rfr<source_question>;
  
  class source_question_from_template;
  using source_question_from_template_p = ptr<source_question_from_template>;
  using source_question_from_template_r = rfr<source_question_from_template>;
  
  class source_template_question;
  using source_template_question_p = ptr<source_template_question>;
  using source_template_question_r = rfr<source_template_question>;
  
  class source_template_question_with_options;
  using source_template_question_with_options_p = ptr<source_template_question_with_options>;
  using source_template_question_with_options_r = rfr<source_template_question_with_options>;
  
  class source_template_question_localization;
  using source_template_question_localization_p = ptr<source_template_question_localization>;
  using source_template_question_localization_r = rfr<source_template_question_localization>;
  
  class source_template_question_localization_with_options;
  using source_template_question_localization_with_options_p = ptr<source_template_question_localization_with_options>;
  using source_template_question_localization_with_options_r = rfr<source_template_question_localization_with_options>;
  
  class localized_question;
  using localized_question_p = ptr<localized_question>;
  using localized_question_r = rfr<localized_question>;

  class answer_data;
  using answer_data_p = ptr<answer_data>;
  using answer_data_r = rfr<answer_data>;
  
  class answer_data_multiple_choices;
  using answer_data_multiple_choices_p = ptr<answer_data_multiple_choices>;
  using answer_data_multiple_choices_r = rfr<answer_data_multiple_choices>;
  
  class localized_answer_data_and_more_payload;
  using localized_answer_data_and_more_payload_p = ptr<localized_answer_data_and_more_payload>;
  using localized_answer_data_and_more_payload_r = rfr<localized_answer_data_and_more_payload>;

  class localized_answer_data;
  using localized_answer_data_p = ptr<localized_answer_data>;
  using localized_answer_data_r = rfr<localized_answer_data>;

  class localized_answer_data_with_options;
  using localized_answer_data_with_options_p = ptr<localized_answer_data_with_options>;
  using localized_answer_data_with_options_r = rfr<localized_answer_data_with_options>;
  
  class localized_answer_data_multiple_choices;
  using localized_answer_data_multiple_choices_p = ptr<localized_answer_data_multiple_choices>;
  using localized_answer_data_multiple_choices_r = rfr<localized_answer_data_multiple_choices>;
  
  // Ontology (persistent).

  class function;
  using function_p = ptr<function>;
  using function_r = rfr<function>;
  
  class option;
  using option_p = ptr<option>;
  using option_r = rfr<option>;
  
  class transition;
  using transition_p = ptr<transition>;
  using transition_r = rfr<transition>;

  class question_body;
  using question_body_p = ptr<question_body>;
  using question_body_r = rfr<question_body>;

  // Start of specializations for the question body type.
  
  class question_body_message;
  using question_body_message_p = ptr<question_body_message>;
  using question_body_message_r = rfr<question_body_message>;

  class question_body_input;
  using question_body_input_p = ptr<question_body_input>;
  using question_body_input_r = rfr<question_body_input>;

  class question_body_with_options;
  using question_body_with_options_p = ptr<question_body_with_options>;
  using question_body_with_options_r = rfr<question_body_with_options>;

  class question_body_select;
  using question_body_select_p = ptr<question_body_select>;
  using question_body_select_r = rfr<question_body_select>;

  class question_body_multiple_choices;
  using question_body_multiple_choices_p = ptr<question_body_multiple_choices>;
  using question_body_multiple_choices_r = rfr<question_body_multiple_choices>;

  class question_body_select_at_most;
  using question_body_select_at_most_p = ptr<question_body_select_at_most>;
  using question_body_select_at_most_r = rfr<question_body_select_at_most>;

  class question_body_select_limit;
  using question_body_select_limit_p = ptr<question_body_select_limit>;
  using question_body_select_limit_r = rfr<question_body_select_limit>;

  class question_body_rank_at_most;
  using question_body_rank_at_most_p = ptr<question_body_rank_at_most>;
  using question_body_rank_at_most_r = rfr<question_body_rank_at_most>;

  class question_body_rank_limit;
  using question_body_rank_limit_p = ptr<question_body_rank_limit>;
  using question_body_rank_limit_r = rfr<question_body_rank_limit>;

  // End of specializations for the question body type.

  class question;
  using question_p = ptr<question>;
  using question_r = rfr<question>;

  class question_with_body;
  using question_with_body_p = ptr<question_with_body>;
  using question_with_body_r = rfr<question_with_body>;

  class question_begin_loop;
  using question_begin_loop_p = ptr<question_begin_loop>;
  using question_begin_loop_r = rfr<question_begin_loop>;
  
  class question_end_loop;
  using question_end_loop_p = ptr<question_end_loop>;
  using question_end_loop_r = rfr<question_end_loop>;
  
  class question_from_template;
  using question_from_template_p = ptr<question_from_template>;
  using question_from_template_r = rfr<question_from_template>;

  class template_question_category;
  using template_question_category_p = ptr<template_question_category>;
  using template_question_category_r = rfr<template_question_category>;
  
  class template_question;
  using template_question_p = ptr<template_question>;
  using template_question_r = rfr<template_question>;
  
  class questionnaire;
  using questionnaire_p = ptr<questionnaire>;
  using questionnaire_r = rfr<questionnaire>;

  class campaign;
  using campaign_p = ptr<campaign>;
  using campaign_r = rfr<campaign>;

  // Localization.
  
  class option_localization;
  using option_localization_p = ptr<option_localization>;
  using option_localization_r = rfr<option_localization>;

  class question_localization_body;
  using question_localization_body_p = ptr<question_localization_body>;
  using question_localization_body_r = rfr<question_localization_body>;

  // Specializations of question localization.
  // As there is a localization type per question type, it means that all question types must
  // have an exact equivalent here. Intermediary types to share code among question bodies
  // do not all need an equivalent here as we store only labels.
  
  class question_localization_body_message;
  using question_localization_body_message_p = ptr<question_localization_body_message>;
  using question_localization_body_message_r = rfr<question_localization_body_message>;
  
  class question_localization_body_input;
  using question_localization_body_input_p = ptr<question_localization_body_input>;
  using question_localization_body_input_r = rfr<question_localization_body_input>;
  
  class question_localization_body_with_options;
  using question_localization_body_with_options_p = ptr<question_localization_body_with_options>;
  using question_localization_body_with_options_r = rfr<question_localization_body_with_options>;
  
  class question_localization_body_select;
  using question_localization_body_select_p = ptr<question_localization_body_select>;
  using question_localization_body_select_r = rfr<question_localization_body_select>;
  
  class question_localization_body_multiple_choices;
  using question_localization_body_multiple_choices_p = ptr<question_localization_body_multiple_choices>;
  using question_localization_body_multiple_choices_r = rfr<question_localization_body_multiple_choices>;
  
  class question_localization_body_select_at_most;
  using question_localization_body_select_at_most_p = ptr<question_localization_body_select_at_most>;
  using question_localization_body_select_at_most_r = rfr<question_localization_body_select_at_most>;
  
  class question_localization_body_select_limit;
  using question_localization_body_select_limit_p = ptr<question_localization_body_select_limit>;
  using question_localization_body_select_limit_r = rfr<question_localization_body_select_limit>;
  
  class question_localization_body_rank_at_most;
  using question_localization_body_rank_at_most_p = ptr<question_localization_body_rank_at_most>;
  using question_localization_body_rank_at_most_r = rfr<question_localization_body_rank_at_most>;
  
  class question_localization_body_rank_limit;
  using question_localization_body_rank_limit_p = ptr<question_localization_body_rank_limit>;
  using question_localization_body_rank_limit_r = rfr<question_localization_body_rank_limit>;
  
  // End of specializations of question localization body.
  
  class question_localization;
  using question_localization_p = ptr<question_localization>;
  using question_localization_r = rfr<question_localization>;

  class template_question_localization;
  using template_question_localization_p = ptr<template_question_localization>;
  using template_question_localization_r = rfr<template_question_localization>;

  class questionnaire_localization;
  using questionnaire_localization_p = ptr<questionnaire_localization>;
  using questionnaire_localization_r = rfr<questionnaire_localization>;

  // Interview.

  class choice;
  using choice_p = ptr<choice>;
  using choice_r = rfr<choice>;

  class answer_body;
  using answer_body_p = ptr<answer_body>;
  using answer_body_r = rfr<answer_body>;

  // Start of specializations for the answer body type.
  
  class answer_body_message;
  using answer_body_message_p = ptr<answer_body_message>;
  using answer_body_message_r = rfr<answer_body_message>;

  class answer_body_input;
  using answer_body_input_p = ptr<answer_body_input>;
  using answer_body_input_r = rfr<answer_body_input>;

  class answer_body_select;
  using answer_body_select_p = ptr<answer_body_select>;
  using answer_body_select_r = rfr<answer_body_select>;

  class answer_body_multiple_choices;
  using answer_body_multiple_choices_p = ptr<answer_body_multiple_choices>;
  using answer_body_multiple_choices_r = rfr<answer_body_multiple_choices>;

  class answer_body_select_at_most;
  using answer_body_select_at_most_p = ptr<answer_body_select_at_most>;
  using answer_body_select_at_most_r = rfr<answer_body_select_at_most>;

  class answer_body_select_limit;
  using answer_body_select_limit_p = ptr<answer_body_select_limit>;
  using answer_body_select_limit_r = rfr<answer_body_select_limit>;

  class answer_body_rank_at_most;
  using answer_body_rank_at_most_p = ptr<answer_body_rank_at_most>;
  using answer_body_rank_at_most_r = rfr<answer_body_rank_at_most>;

  class answer_body_rank_limit;
  using answer_body_rank_limit_p = ptr<answer_body_rank_limit>;
  using answer_body_rank_limit_r = rfr<answer_body_rank_limit>;

  // End of specializations for the answer body type.
  
  class answer;
  using answer_p = ptr<answer>;
  using answer_r = rfr<answer>;

  class entry;
  using entry_p = ptr<entry>;
  using entry_r = rfr<entry>;

  class entry_answer;
  using entry_answer_p = ptr<entry_answer>;
  using entry_answer_r = rfr<entry_answer>;

  class entry_begin_loop;
  using entry_begin_loop_p = ptr<entry_begin_loop>;
  using entry_begin_loop_r = rfr<entry_begin_loop>;

  class entry_end_loop;
  using entry_end_loop_p = ptr<entry_end_loop>;
  using entry_end_loop_r = rfr<entry_end_loop>;

  class interview;
  using interview_p = ptr<interview>;
  using interview_r = rfr<interview>;

  class the_stack;
    
  // A template localization does not have any link to the question from template. We carry both.
  struct template_localization
  {
    template_question_localization_r localization;
    question_from_template_r question;
  };
  
  // The question localization and the template localization cannot be non null at the same time.
  // If the question localization is null and the template localization null, it is the end of
  // the interview.
  // If the template localization is non null, then the two smart pointers in the pair are non
  // null too.
  using localizations = std::variant<question_localization_r, template_localization>;

  class function: public element<>
  {
    HX2A_ELEMENT(function, type_tag<"text">, element,
		 (_parameters, _code));
  public:

    using parameters_type = link_list<question, "p">;
    
    function(const string& code):
      _parameters(*this),
      _code(*this, code)
    {
    }

    parameters_type::const_iterator parameters_cbegin() const { return _parameters.cbegin(); }
    
    parameters_type::const_iterator parameters_cend() const { return _parameters.cend(); }

    void push_parameter_back(const question_r& q){
      _parameters.push_back(q);
    }

    const string& get_code() const { return _code.get(); }

    bool empty() const { return _code.get().empty(); }
    
    void push_argument(const string& var, const json::value& v){
      _code << js_variable(var, v);
    }

    // For parametric texts.
    // The language is pushed so that the code can use it.
    // After this function all the arguments are cleared and must be pushed again.
    // We could also add loop variables by passing the stack.
    // Cannot be const.
    json_dom::value call(language_t);

    // For transitions (more frugal).
    // After this function all the arguments are cleared and must be pushed again.
    json_dom::value call() const {
      return slot_js_run(_code);
    }

    // No performance effect, just to check that the code is correct.
    void compile() const { _code.compile(); }

    function_r clone() const {
      function_r f = make<function>(_code);

      for (const auto& q: _parameters){
	HX2A_ASSERT(q);
	f->push_parameter_back(*q);
      }

      return f;
    }

    // Returns true of the question is in the parameters.
    bool uses_as_parameter(const question_r& q) const {
      for (const auto& p: _parameters){
	HX2A_ASSERT(p);

	if (p == q){
	  return true;
	}
      }

      return false;
    }

  private:

    parameters_type _parameters;
    slot_js<"c"> _code;
  };

  // Needs to be an anchor to be the target of the link on a localization and on an answer.
  class option: public anchor<>
  {
    HX2A_ANCHOR(option, type_tag<"option">, anchor,
		(_has_comment));
  public:

    option(bool has_comment):
      _has_comment(*this, has_comment)
    {
    }
    
    option_r clone() const {
      return make<option>(_has_comment);
    }

    bool has_comment() const { return _has_comment; }

  private:

    slot<bool, "hc"> _has_comment;
  };

  class transition: public element<>
  {
    HX2A_ELEMENT(transition, type_tag<"transition">, element,
		 (_condition, _destination));
  public:

    using parameters_type = function::parameters_type;

    // No condition is fine, it'll always be true.
    transition(
	       const question_r& destination // The question to transition to.
	       ):
      _condition(*this),
      _destination(*this, destination)
    {
    }

    transition(
	       const function_r& condition,
	       const question_r& destination // The question to transition to.
	       ):
      _condition(*this, condition),
      _destination(*this, destination)
    {
    }

    parameters_type::const_iterator parameters_cbegin() const {
      HX2A_ASSERT(_condition);
      return _condition->parameters_cbegin();
    }
    
    parameters_type::const_iterator parameters_cend() const {
      HX2A_ASSERT(_condition);
      return _condition->parameters_cend();
    }

    void push_parameter_back(const question_r& q){
      HX2A_ASSERT(_condition);
      _condition->push_parameter_back(q);
    }
    
    function_p get_condition() const {
      return _condition;
    }

    string_view get_condition_code() const {
      return _condition ? _condition->get_code() : string_view{};
    }
    
    // The ctor guarantees it is never null.
    question_r get_destination() const { return *_destination; }

    // With a specified destination.
    transition_r clone(const question_r& q) const {
      HX2A_ASSERT(_condition);
      auto t = make<transition>(_condition->clone(), q);
      auto i = _condition->parameters_cbegin();
      auto e = _condition->parameters_cend();
      
      while (i != e){
	HX2A_ASSERT(*i);
	t->push_parameter_back(**i);
	++i;
      }
      
      return t;
    }

    // As-is.
    transition_r clone() const {
      HX2A_ASSERT(_destination);
      return clone(*_destination);
    }

    // Transitions are not allowed to use the language.
    // Returns a non null question smart pointer when the transition is valid.
    // Cannot be const.
    question_p run(const the_stack&, time_t start_timestamp);

    void check_condition() const {
      HX2A_ASSERT(_condition);
      _condition->compile();
    }

    // Returns true of the question is in the parameters.
    bool is_driven_by(const question_r& q) const {
      HX2A_ASSERT(_condition);
      return _condition->uses_as_parameter(q);
    }

  private:

    // This is the condition under which the transition will be made to the question
    // the link points at.
    // It is a JavaScript piece of code that needs to evaluate to true, according to
    // the json::is_true function called on the result of the run function called on
    // the slot_js (it returns true if the JavaScript value is the true boolean or
    // a non null numeric value.
    // All the responses to previous questions are injected in the JavaScript code
    // snipped as available variables to be used to calculate the condition.
    // So the following JavaScript code works:
    // qu2 == 2
    // If "qu2" is the label of the current question and the transition applies when
    // the option is 2.
    own<function, "c"> _condition;
    link<question, "d"> _destination;
  };

  // To keep a mapping between cloned questions and clone questions.
  using cloned_to_clone_questions_map =
    ::std::unordered_map<
      question*, // The key, the cloned question.
      question* // The data, the clone question.
    >;

  using loop_nest = ::std::vector<question_begin_loop_r>;
    
  class question_info
  {
  public:

    // Index and level from 0.
    question_info(size_t index, const loop_nest& loop_nest, const question_begin_loop_p& mbl):
      _index(index),
      _loop_nest(loop_nest), // We make a copy.
      _matching_begin_loop(mbl) // Only for end loops.
    {
    }

    size_t get_index() const { return _index; }

    const loop_nest& get_loop_nest() const { return _loop_nest; }

    question_begin_loop_p get_parent_begin_loop() const {
      if (!_loop_nest.empty()){
	return _loop_nest.back();
      }

      return {};
    }

    question_begin_loop_p get_matching_begin_loop() const { return _matching_begin_loop; }
    
  private:
    
    size_t _index;
    // The path of begin loops loop the question is part of.
    // The immediate parent is at the end.
    // A begin loop is not attached to itself, but to its parent begin loop, if any.
    // An end loop is not attached to its matching begin loop, but to its parent begin loop, if any.
    ::std::vector<question_begin_loop_r> _loop_nest;
    // Only set for end loops. Null in all other cases.
    question_begin_loop_p _matching_begin_loop;
  };
  
  // To keep a map of question information and questions by their number by label.
  using question_infos_by_label_map = ::std::unordered_map<string, pair<question_info, question*>>;

  // To keep a map of question informations by question pointer.
  using leveled_questionnaire = ::std::unordered_map<question*, question_info>;

  // The following type and its derived types do not bear any language-oriented labels. These are abstract
  // data. Labels can be found on localization types.
  class question_body: public element<>
  {
    HX2A_ELEMENT(question_body, type_tag<"q_body">, element,
		 (_text_functions, _style));
  public:

    // Using a vector as we want direct access to an element when computing parametric texts.
    using text_functions_type = own_vector<function, "f">;
    
    question_body(const string& style):
      _text_functions(*this),
      _style(*this, style)
    {
    }

    const string& get_style() const { return _style; }

    void update(const string& style){
      _style = style;
    }

    text_functions_type::const_iterator text_functions_cbegin() const { return _text_functions.cbegin(); }
    
    text_functions_type::const_iterator text_functions_cend() const { return _text_functions.cend(); }

    void push_text_function_back(const function_r& f){ _text_functions.push_back(f); }
    
    // An ontology type cannot be abstract. Dummy definition.
    virtual question_body_r clone() const {
      HX2A_ASSERT(false);
      return make<question_body>("");
    }

    // Applies all the functions, if any, and replaces all the calls with the output of the corresponding code.
    // Returns the result.
    // label provided for clearer error messages.
    string calculate_text(
			  const string& label,
			  const the_stack&,
			  language_t,
			  const string& text
			  ) const;

    virtual bool can_be_final() const { return false; }

    bool is_impacted_by(const question_r& q) const {
      for (const auto& f: _text_functions){
	HX2A_ASSERT(f);

	if (f->uses_as_parameter(q)){
	  return true;
	}
      }

      return false;
    }

  private:

    // The optional functions that allow to calculate parametric texts.
    text_functions_type _text_functions;
    // Positioned on the body because questions from template do not have a style, it is coming
    // from the template question.
    // It might have been an enumerate, but to maintain it properly it would mean it is on each body.
    // Instead of a forced style, we might want to implement a smarter GUI, choosing the right widget
    // as a function of the question. E.g. if in a select there are too many options, it does not choose
    // a radio select but a drop down.
    slot<string, "s"> _style;
  };

  class question_body_message: public question_body
  {
    HX2A_ELEMENT(question_body_message, type_tag<"q_b_message">, question_body,
		 ());
  public:

    using question_body::question_body;

    question_body_r clone() const override {
      return make<question_body_message>(get_style());
    }

    bool can_be_final() const override { return true; }
  };

  class question_body_with_comment: public question_body
  {
    HX2A_ELEMENT(question_body_with_comment, type_tag<"q_body_with_comment">, question_body,
		 (_has_comment));
  public:

    question_body_with_comment(const string& style, bool has_comment):
      question_body(style),
      _has_comment(*this, has_comment)
    {
    }

    bool has_comment() const {
      return _has_comment;
    }

    void update(const string& style, bool has_comment){
      question_body::update(style);
      _has_comment = has_comment;
    }
    
  private:
    
    slot<bool, "hc"> _has_comment;
  };
  
  // A question with an input field, and an optional comment with another input field.
  // We do not want to create variations of all the question types with an input box at the end and
  // the same question types without. So its presence is optional and governed by the "has comment"
  // boolean.
  class question_body_input: public question_body_with_comment
  {
    HX2A_ELEMENT(question_body_input, type_tag<"q_b_input">, question_body_with_comment,
		 (_optional));
  public:

    question_body_input(const string& style, bool has_comment, bool optional):
      question_body_with_comment(style, has_comment),
      _optional(*this, optional)
    {
    }

    bool is_optional() const { return _optional; }
    
    question_body_r clone() const override {
      return make<question_body_input>(get_style(), has_comment(), is_optional());
    }

  private:

    // Indicates whether an input is mandatory or not.
    slot<bool, "o"> _optional;
  };

  class question_body_with_options: public question_body_with_comment
  {
    HX2A_ELEMENT(question_body_with_options, type_tag<"q_o">, question_body_with_comment,
		 (_options, _randomize));
  public:

    // own_list so that it shrinks automatically when an option is cut.
    using options_type = own_list<option, "o">;

    // The randomize boolean indicates whether the GUI should randomize the options.
    question_body_with_options(const string& style, bool randomize, bool has_comment):
      question_body_with_comment(style, has_comment),
      _options(*this),
      _randomize(*this, randomize)
    {
    }
      
    size_t get_options_size() const { return _options.size(); }

    options_type::const_iterator options_cbegin() const { return _options.cbegin(); }
    options_type::const_iterator options_cend() const { return _options.cend(); }

    void push_option_back(const option_r& c){
      _options.push_back(c);
    }

    void add_options_to(const question_body_with_options_r& to) const {
      // Cloning options.
      for (const auto& c: _options){
	HX2A_ASSERT(c);
	to->push_option_back(c->clone());
      }
    }

    void add_options_to(const source_template_question_with_options_r&) const;

    bool get_randomize() const { return _randomize; }
    
    // No clone of its own.
    
  private:
    
    // own_list so that it shrinks automatically when an option is cut.
    options_type _options;
    // Rather than multiplying the types or making a template type, we have a boolean driving whether options should be
    // displayed randomized or not by the GUI.
    slot<bool, "r"> _randomize;
  };

  class question_body_select: public question_body_with_options
  {
    HX2A_ELEMENT(question_body_select, type_tag<"q_bs">, question_body_with_options,
		 ());
  public:

    using question_body_with_options::question_body_with_options;
    
    question_body_r clone() const override {
      question_body_select_r rtnd = make<question_body_select>(get_style(), get_randomize(), has_comment());
      add_options_to(rtnd);
      return rtnd;
    }
  };

  // Parent class for all questions with options that must be selected up to a limit.
  class question_body_multiple_choices: public question_body_with_options
  {
    HX2A_ELEMENT(question_body_multiple_choices, type_tag<"q_ms">, question_body_with_options,
		 (_limit));
  public:

    // In case limit ends up higher than the number of options, the number of options
    // prevails.
    question_body_multiple_choices(const string& style, bool randomize, bool has_comment, size_t limit):
      question_body_with_options(style, randomize, has_comment),
      _limit(*this, limit)
    {
    }

    size_t get_limit() const { return _limit; }

    void update(const string& style, bool has_comment, size_t limit){
      question_body_with_comment::update(style, has_comment);
      _limit = limit;
    }
    
    // No clone of its own.
    
  private:
    
    slot<size_t, "L"> _limit;
  };

  class question_body_select_at_most: public question_body_multiple_choices
  {
    HX2A_ELEMENT(question_body_select_at_most, type_tag<"q_bsam">, question_body_multiple_choices,
		 ());
  public:

    using question_body_multiple_choices::question_body_multiple_choices;
    
    question_body_r clone() const override {
      question_body_select_at_most_r rtnd = make<question_body_select_at_most>(get_style(), get_randomize(), has_comment(), get_limit());
      add_options_to(rtnd);
      return rtnd;
    }
  };

  class question_body_select_limit: public question_body_multiple_choices
  {
    HX2A_ELEMENT(question_body_select_limit, type_tag<"q_bsl">, question_body_multiple_choices,
		 ());
  public:

    using question_body_multiple_choices::question_body_multiple_choices;
    
    question_body_r clone() const override {
      question_body_select_limit_r rtnd = make<question_body_select_limit>(get_style(), get_randomize(), has_comment(), get_limit());
      add_options_to(rtnd);
      return rtnd;
    }
  };

  class question_body_rank_at_most: public question_body_multiple_choices
  {
    HX2A_ELEMENT(question_body_rank_at_most, type_tag<"q_bram">, question_body_multiple_choices,
		 ());
  public:

    using question_body_multiple_choices::question_body_multiple_choices;
    
    question_body_r clone() const override {
      question_body_rank_at_most_r rtnd = make<question_body_rank_at_most>(get_style(), get_randomize(), has_comment(), get_limit());
      add_options_to(rtnd);
      return rtnd;
    }
  };

  class question_body_rank_limit: public question_body_multiple_choices
  {
    HX2A_ELEMENT(question_body_rank_limit, type_tag<"q_brl">, question_body_multiple_choices,
		 ());
  public:

    using question_body_multiple_choices::question_body_multiple_choices;
    
    question_body_r clone() const override {
      question_body_rank_limit_r rtnd = make<question_body_rank_limit>(get_style(), get_randomize(), has_comment(), get_limit());
      add_options_to(rtnd);
      return rtnd;
    }
  };

  // The first question needs a label, although no transition will have it as a destination. This label is
  // useful for instance for localizations, as localizations are not uploaded in the same order as the questions
  // and refer to questions with their label..
  // We're not parsing the JavaScript conditions code, so questions have a label to identify them uniquely.
  // Question types that can be final questions do not possess transitions. Yet, it does not mean that the
  // corresponding question type NEVER has transitions. Indeed, a potentially final question type can appear
  // in the middle of a questionnaire, with transitions. In that case it is not final.
  // There are potentially many different potentially final question types. A message saying "Thank you", a
  // video showing "Thank you" or fireworks...
  class question: public anchor<>
  {
    HX2A_ANCHOR(question, type_tag<"q">, anchor,
		(_label, _transitions));
  public:

    // own_list so that it shrinks automatically when a transition is cut.
    using transitions_type = own_list<transition, "t">;

    enum loop_type_t {regular, begin_loop, end_loop};

    // Must check that the label is a string that is a valid name for a JavaScript
    // variable, as it'll be injected in the condition of all the transitions of
    // subsequent questions. As the full regular expression for valid JavaScript
    // variables is far too complex, we restrict the valid labels to the following
    // regular expression, which describes a subset of valid JavaScript variable
    // names:
    // [a-zA-Z$][0-9a-zA-Z_$]*
    question(const string& label):
      _label(*this, label),
      _transitions(*this)
    {
      if (!validate_label(label)){
	throw question_label_is_invalid(label);
      }
    }

    // The clone function does not clone the transitions, as transitions must be added in a second pass on the
    // questionnaire.
    // Dummy function, ontology types cannot be abstract.
    virtual question_r clone() const {
      HX2A_ASSERT(false);
      return make<question>("");
    }

    // To clone only the transitions of this into the question given in argument.
    // Passing the map that allows to find the new transitions destinations.
    void clone_transitions_to(const question_r&, const cloned_to_clone_questions_map&);

    const string& get_label() const { return _label; }

    // Dummy body.
    virtual string get_style() const {
      HX2A_ASSERT(false);
      return "";
    }

    size_t transitions_size() const { return _transitions.size(); }
    
    transitions_type::const_iterator transitions_cbegin() const { return _transitions.cbegin(); }
    transitions_type::const_iterator transitions_cend() const { return _transitions.cend(); }

    void push_transition_back(const transition_r& t){
      _transitions.push_back(t);
    }

    virtual question_body_r get_body() const {
      HX2A_ASSERT(false);
      return make<question_body>("");
    }

    // Returns the optional next question, after running transitions.
    question_r run_transitions(const the_stack&, time_t start_timestamp) const;

    void check_conditions() const {
      for (const auto& t: _transitions){
	t->check_condition();
      }
    }

    virtual bool supports_localization() const { return false; }

    virtual bool supports_answer() const { return true; }

    virtual loop_type_t get_loop_type() const { return regular; }
    
    virtual bool can_be_final() const { return false; }

    virtual bool is_final() const {
      return !_transitions.size() && can_be_final();
    }

    void add_transitions_to(const source_question_r&) const;

    // Usually source questions are built from their localization, as source questions come with one localization. But not all
    // questions support localization (e.g., question from template), so the function below is called instead in these cases.
    // This implementation is dummy.
    virtual source_question_r make_source_question(language_t) const;

    // Stacks the question if it is a begin loop.
    // Unstacks if the question is an end loop.
    // Doing nothing otherwise.
    // Dummy.
    virtual void update_loop_nest(loop_nest&) const {
    }

    // Same but lighter.
    // Dummy.
    virtual void update_loop_counter(size_t&) const {
    }

    // Checks that the label is acceptable as a JavaScript variable and that it is not reserved.
    static bool validate_label(const string&);

    // Indicates whether the question given in argument "drives" (is a parameter of) one of the transitions.
    bool is_driven_by(const question_r& q) const {
      for (const auto& t: _transitions){
	HX2A_ASSERT(t);
	
	if (t->is_driven_by(q)){
	  return true;
	}
      }

      return false;
    }

    // Returns true if this is a begin loop iterating on the answer to the question given in argument or if
    // this is a question with body with a calculated text using the answer to the question given in argument
    // as a parameter.
    // To be precise it does not mean that the question  is impacted, it means that it is POTENTIALLY impacted.
    // Indeed, the change in the answer corresponding to the quesiton given in argument might be of no consequence
    // to this.
    virtual bool is_impacted_by(const question_r&) const {
      return false;
    }
    
  private:

    slot<string, "l"> _label;
    // Even if the source does not have any transitions, a catch-all transition will be
    // added with an empty condition. It'll transition to the next question.
    transitions_type _transitions;
  };

  class question_with_body: public question
  {
    HX2A_ANCHOR(question_with_body, type_tag<"q_wb">, question,
		(_body));
  public:

    question_with_body(const string& label, const question_body_r& body):
      question(label),
      _body(*this, body)
    {
    }

    string get_style() const override {
      HX2A_ASSERT(_body);
      return _body->get_style();
    }
    
    question_body_r get_body() const final {
      HX2A_ASSERT(_body);
      return *_body;
    }
    
    question_r clone() const override {
      HX2A_ASSERT(_body);
      return make<question_with_body>(get_label(), *_body);
    }

    virtual bool supports_localization() const override { return true; }
    
    bool can_be_final() const override {
      HX2A_ASSERT(_body);
      return _body->can_be_final();
    }

    bool is_impacted_by(const question_r& q) const override {
      HX2A_ASSERT(_body);
      return _body->is_impacted_by(q);
    }

  private:
    
    own<question_body, "b"> _body;
  };

  class question_from_template: public question
  {
    HX2A_ANCHOR(question_from_template, type_tag<"q_ft">, question,
		(_template_question));
  public:

    question_from_template(const string& label, const template_question_r& tq):
      question(label),
      _template_question(*this, tq)
    {
    }

    template_question_r get_template_question() const {
      HX2A_ASSERT(_template_question);
      return *_template_question;
    }

    question_body_r get_body() const override;

    question_r clone() const override {
      HX2A_ASSERT(_template_question);
      return make<question_from_template>(get_label(), *_template_question);
    }

    bool can_be_final() const override;

    source_question_r make_source_question(language_t) const override;
    
  private:

    link<template_question, "T"> _template_question;
  };

  class question_begin_loop: public question
  {
    HX2A_ANCHOR(question_begin_loop, type_tag<"q_bl">, question,
		(_operand_question, _variable, _operand));
  public:

    question_begin_loop(const string& label, const question_r& q, const string& variable, const string& operand):
      question(label),
      _operand_question(*this, q),
      _variable(*this, variable),
      _operand(*this, operand)
    {
      // Checking that the question is a question that admits an answer. All do, except loop questions.
      if (!q->supports_answer()){
	throw question_begin_loop_refers_to_unanswerable_question(label);
      }
    
      if (!validate_label(_variable)){
	throw question_begin_loop_variable_is_invalid(label);
      }

      if (operand.empty()){
	throw question_begin_loop_has_no_operand(label);
      }
    }

    question_r get_operand_question() const {
      HX2A_ASSERT(_operand_question);
      return *_operand_question;
    }

    const string& get_variable() const { return _variable; }

    const string& get_operand() const { return _operand; }

    source_question_r make_source_question(language_t) const override;
    
    void update_loop_nest(loop_nest& ln) const override {
      ln.push_back(*this);
    }

    void update_loop_counter(size_t& counter) const override {
      ++counter;
    }
    
    bool supports_answer() const override { return false; }

    loop_type_t get_loop_type() const override { return begin_loop; }

    bool is_impacted_by(const question_r& q) const override {
      return _operand_question == q;
    }

  private:

    // The previous question whose answer is to iterate upon.
    link<question, "q"> _operand_question;
    // The name of the JavaScript variable which will contain each element of the JSON vector
    // pointed at by the operand.
    slot<string, "v"> _variable;
    // The code yielding a JSON vector from the answer to iterate upon. In most cases it'll simply be a path.
    // Must have the "R=" prefix.
    slot<string, "o"> _operand;
  };

  class question_end_loop: public question
  {
    HX2A_ANCHOR(question_end_loop, type_tag<"q_el">, question,
		());
  public:

    question_end_loop(const string& label):
      question(label)
    {
    }
    
    source_question_r make_source_question(language_t) const override;
    
    void update_loop_nest(loop_nest& ln) const override {
      if (ln.empty()){
	throw question_loop_is_not_balanced(get_label());
      }
      
      ln.pop_back();
    }

    void update_loop_counter(size_t& counter) const override {
      if (!counter){
	throw question_loop_is_not_balanced(get_label());
      }

      --counter;
    }
    
    bool supports_answer() const override { return false; }
    
    loop_type_t get_loop_type() const override { return end_loop; }
  };

  // No unicity of the name whatsoever is provided.
  class template_question_category: public root<>
  {
    HX2A_ROOT(template_question_category, type_tag<"tq_c">, 1, root,
	      (_name, _parent));
  public:

    template_question_category(const string& name, const template_question_category_p& par):
      _name(*this, name),
      _parent(*this, par)
    {
    }

    const string& get_name() const { return _name; }

    void set_name(const string& name){ _name = name; }

    void update(const string& name){
      _name = name;
    }

    // It'll be null for a root category.
    template_question_category_p get_parent() const { return _parent; }

  private:

    slot<string, "n"> _name;
    link<template_question_category, "p"> _parent;
  };

  // The label of the question is the label of the template, not the label of the question in the questionnaire.
  class template_question: public root<>
  {
    HX2A_ROOT(template_question, type_tag<"tq">, 1, root,
	      (_category, _label, _body));
  public:

    template_question(
		      const template_question_category_r& category,
		      const string& label,
		      const question_body_r& body):
      _category(*this, category),
      _label(*this, label),
      _body(*this, body)
    {
    }

    template_question_category_r get_category() const { return *_category; }

    void set_category(const template_question_category_r& tqc){ _category = tqc; }

    question_body_r get_body() const {
      HX2A_ASSERT(_body);
      return *_body;
    }

    void set_body(const question_body_r& b){ _body = b; }

    const string& get_label() const { return _label; }

    // It'll check that the label does not already exist.
    void update(const string& label);

    bool can_be_final() const { return _body->can_be_final(); }

    // The label is the template name.
    static template_question_p find(const db::connector& c, const string& label);

  private:

    link<template_question_category, "c"> _category;
    // The label is the template unique name.
    slot<string, "l"> _label;
    // There is no need for a specific template question body, we can reuse the regular question body.
    own<question_body, "q"> _body;
  };

  // A questionnaire is automatically locked when a campaign is created.
  // It prevents the questionnaire and its localizations to be modified.
  // Clone the questionnaire and perform modifications in case modifications are necessary.
  class questionnaire: public root<>
  {
    HX2A_ROOT(questionnaire, type_tag<"qq">, 1.1, root,
	      (_code, _name, _logo, _questions, _locked, _change_count));
  public:

    // The list is ordered. All questions should have transitions to subsequent questions, no previous one.
    // own_list so that it shrinks automatically when a question is cut.
    using questions_type = own_list<question, "q">;

    questionnaire(
		  const string& code,
		  const string& name,
		  const string& logo
		  ):
      _code(*this, code),
      _name(*this, name),
      _logo(*this, logo),
      _questions(*this),
      _locked(*this, false),
      // The questionnaire is built with a change count of 1 so that the questionnaire localization check
      // happens at least once. The questionnaire localization change count is initialized at 0.
      _change_count(*this, 1)
    {
    }

    const string& get_code() const { return _code; }

    const string& get_name() const { return _name; }

    const string& get_logo() const { return _logo; }

    // Returns the number of questions.
    size_t size() const {
      return _questions.size();
    }

    // Includes the check on orphan questions.
    // Returns immediately if the questionnaire is locked.
    void check() const;
    
    // Checks whether the questionnaire does not have any question other than the first which is not
    // the target of a transition.
    // Throws exception question_is_orphan when an orphan is found.
    void check_orphans() const;

    // This is pending Metaspex generating a clone function automatically.
    // As questionnaire are locked as soon as a campaign is created, the only solution to update them subsequently
    // is to clone them.
    questionnaire_r clone(
			  const db::connector& c,
			  const string& new_code,
			  const string& new_name,
			  const string& new_logo
			  ) const;

    // Populates the map.
    // If it's just for finding a single question, have a look at find question.
    void dump(question_infos_by_label_map&);

    // Similar, different map.
    void dump(leveled_questionnaire&);
    
    bool is_locked() const { return _locked; }

    // Can be called repeatedly without marking the questionnaire as modified.
    void lock(){
      _locked = true;
    }

    void check_lock() const {
      if (is_locked()){
	throw questionnaire_is_locked();
      }
    }

    unsigned int get_change_count() const { return _change_count; }
    
    void push_question_back(const question_r& q){
      check_lock();
      _questions.push_back(q);
      touch();
    }

    // Returns nullptr if not found.
    question_p find_question(const string& label) const {
      for (const auto& q: _questions){
	if (q->get_label() == label){
	  return q;
	}
      }

      return {};
    }
    
    question_r get_first_question() const {
      HX2A_ASSERT(_questions.size());
      question_p if_fq = *_questions.cbegin();
      HX2A_ASSERT(if_fq);
      // It cannot be a loop.
      question_r fq = *if_fq;
      HX2A_ASSERT(fq->supports_localization());
      return fq;
    }

    // Used during questionnaire source compilation or during the cloning process, no need to check the lock
    // or incrementing the change count.
    questions_type::iterator questions_begin(){
      return _questions.begin();
    }

    questions_type::iterator questions_end(){
      return _questions.end();
    }

    questions_type::const_iterator questions_cbegin() const {
      return _questions.cbegin();
    }

    questions_type::const_iterator questions_cend() const {
      return _questions.cend();
    }

    // Returns the rank of the question supplied (from 1 to the size of the questionnaire).
    // The size of the questionnaire is the number of questions.
    // If the question is not found, the rank returned will be the size of the questionnaire.
    size_t get_question_rank(const question_r& q) const {
      size_t rank = 1;
      const string& l = q->get_label();

      for (const auto& qq: _questions){
	if (qq->get_label() == l){
	  break;
	}

	++rank;
      }

      return rank;
    }

    // An integral number between 0 and 100. As the last question is just a message, the last question
    // is reported as 100%.
    progress_t get_progress(const question_r& q) const {
      size_t qs = _questions.size();

      if (!qs){
	// There is nothing to do, we're at 100 per cent. In principle that case should not happen, a
	// questionnaire is valid only if there is at least a final message.
	return 100;
      }
      
      return ((float)get_question_rank(q) / (float)_questions.size()) * 100.0;
    }
    
  private:

    void touch(){
      _change_count = _change_count + 1;
    }

    slot<string, "c"> _code;
    // Not localized, just for search.
    slot<string, "n"> _name;
    slot<string, "l"> _logo;
    questions_type _questions;
    slot<bool, "L"> _locked;
    // Meant for instance to avoid spending time checking a localization if it has already been checked.
    slot<unsigned int, "cc"> _change_count;
  };

  class campaign: public root<>
  {
    HX2A_ROOT(campaign, type_tag<"camp">, 1, root,
	      (_name, _questionnaire, _start, _duration, _interview_lifespan, _end));
  public:

    // A 0 start means that the campaign starts immediately.
    // A 0 duration means that the campagn is unlimited in duration.
    // Creating a campaign checks the questionnaire (in particular that it no has orphans) if it was never locked.
    // It locks the questionnaire, even if the campaign is not yet active.
    campaign(
	     const string& name,
	     const questionnaire_r& q,
	     time_t start = 0,
	     time_t duration = 0,
	     time_t interview_lifespan = 0 // Meaning infinite.
	     ):
      _name(*this, name),
      _questionnaire(*this, q),
      _start(*this, start),
      _duration(*this, duration),
      _interview_lifespan(*this, interview_lifespan),
      _end(*this)
    {
      q->check();
      _end = start + duration;
      q->lock();
    }

    void check_active() const {
      if (_start){
	time_t now = time();
	
	if (now < _start){
	  throw campaign_is_not_yet_active();
	}

	if (_duration && now > _end){
	  throw campaign_expired();
	}
      }
    }

    questionnaire_r get_questionnaire() const { return *_questionnaire; }

    const string& get_name() const { return _name; }

    time_t get_start() const { return _start; }

    time_t get_duration() const { return _duration; }

    time_t get_interview_lifespan() const { return _interview_lifespan; }

    time_t get_end() const { return _end; }

    void update(
		const string& name,
		const questionnaire_r& quest,
		time_t start,
		time_t duration,
		time_t interview_lifespan
		){
      _name = name;
      _questionnaire = quest;
      _start = start;
      _duration = duration;
      _interview_lifespan = interview_lifespan;
      _end = start + duration;
    }
    
  private:

    slot<string, "n"> _name;
    link<questionnaire, "q"> _questionnaire;
    slot<time_t, "s"> _start;
    slot<time_t, "d"> _duration;
    slot<time_t, "il"> _interview_lifespan;
    slot<time_t, "e"> _end;
  };

  // Localization.

  // Choices bear a link towards option localizations, so they need to be anchors.
  class option_localization: public anchor<>
  {
    HX2A_ANCHOR(option_localization, type_tag<"option_l10n">, anchor,
		(_option, _label, _comment_label));
  public:

    // First argument only for clear report in case of error.
    option_localization(
			const string& qlabel,
			const option_r& ch,
			const string& label,
			const string& comment_label
			):
      _option(*this, ch),
      _label(*this, label),
      _comment_label(*this, comment_label)
    {
      check(qlabel);
    }

    void check(const string& qlabel) const {
      if (_label.get().empty()){
	throw option_localization_label_is_empty(qlabel);
      }
      
      if (_option->has_comment()){
	if (_comment_label.get().empty()){
	  throw option_localization_comment_does_not_exist(qlabel);
	}
      }
      else if (!_comment_label.get().empty()){
	throw option_localization_comment_is_present(qlabel);
      }
    }

    option_r get_option() const { return *_option; }

    const string& get_label() const { return _label; }

    const string& get_comment_label() const { return _comment_label; }

  private:

    link<option, "o"> _option;
    slot<string, "l"> _label;
    slot<string, "c"> _comment_label;
  };

  class question_localization_body: public element<>
  {
    HX2A_ELEMENT(question_localization_body, type_tag<"q_l10n_body">, element,
		 (_text));
  public:

    question_localization_body(const string& text):
      _text(*this, text)
    {
    }

    const string& get_text() const { return _text; }
    
    string calculate_text(const string& label, const the_stack& ts, language_t lang, const question_body_r& qb) const {
      return qb->calculate_text(label, ts, lang, _text);
    }

    void update(const string& text){ _text = text; }

    // The label is just passed for clear error report.
    // Dummy.
    virtual void check(const string& /* label */, const question_body_r&) const {
      HX2A_ASSERT(false);
    }
    
    // Dummy definition.
    virtual source_question_r make_source_question(const question_r&) const {
      HX2A_ASSERT(false);
      return make<source_question>("");
    }

    // Dummy definition.
    virtual source_template_question_r make_source_template_question(const template_question_localization_r& tql) const;

    // Dummy definition.
    virtual source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const;

    // Dummy.
    virtual localized_question_r make_localized_question(
							 const string& label,
							 const the_stack&,
							 language_t,
							 const string& logo,
							 const string& title,
							 const question_r&,
							 progress_t progress
							 ) const;

  private:

    slot<string, "t"> _text;
  };

  class question_localization_body_message: public question_localization_body
  {
    HX2A_ELEMENT(question_localization_body_message, type_tag<"q_l10n_b_message">, question_localization_body,
		 ());
  public:

    using question_localization_body::question_localization_body;

    void check(const string&, const question_body_r&) const override {}

    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;
    
    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_with_comment: public question_localization_body
  {
    HX2A_ELEMENT(question_localization_body_with_comment, type_tag<"q_l10n_b_with_comment">, question_localization_body,
		 (_comment_label));
  public:

    question_localization_body_with_comment(const string& text,  const string& comment_label):
      question_localization_body(text),
      _comment_label(*this, comment_label)
    {
    }

    const string& get_comment_label() const { return _comment_label; }

    void set_comment_label(const string& cl){ _comment_label = cl; }

    void update(const string& text, const string& comment_label){
      question_localization_body::update(text);
      _comment_label = comment_label;
    }

    // The label is just passed for clear error report.
    void check(const string& label, const question_body_r& qb) const override {
      auto qbwc = checked_cast<question_body_with_comment>(qb);
      
      if (_comment_label.get().empty()){
	if (qbwc->has_comment()){
	  throw question_localization_comment_is_missing(label);
	}
      }
      else{
	if (!qbwc->has_comment()){
	  throw question_localization_comment_is_present(label);
	}
      }

      check_more(label, qb);
    }

    // Dummy definition.
    virtual void check_more(const string& /*label*/, const question_body_r&) const {
      HX2A_ASSERT(false);
    }
    
  private:

    slot<string, "c"> _comment_label;
  };

  class question_localization_body_input: public question_localization_body_with_comment
  {
    HX2A_ELEMENT(question_localization_body_input, type_tag<"q_l10n_b_input">, question_localization_body_with_comment,
		 ());
  public:

    using question_localization_body_with_comment::question_localization_body_with_comment;

    void check_more(const string&, const question_body_r&) const override;
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;
    
    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_with_options: public question_localization_body_with_comment
  {
    HX2A_ELEMENT(question_localization_body_with_options, type_tag<"q_l10n_b_with_options">, question_localization_body_with_comment,
		 (_options));
  public:

    using options_localizations_type = own_list<option_localization, "o">;

    question_localization_body_with_options(const string& text, const string& comment_label):
      question_localization_body_with_comment(text, comment_label),
      _options(*this)
    {
    }

    size_t get_options_size() const { return _options.size(); }

    options_localizations_type::const_iterator options_cbegin() const { return _options.cbegin(); }

    options_localizations_type::const_iterator options_cend() const { return _options.cend(); }

    options_localizations_type::iterator options_begin(){ return _options.begin(); }

    options_localizations_type::iterator options_end(){ return _options.end(); }

    void options_clear(){ _options.clear(); }

    void push_option_back(const option_localization_r& cl){
      _options.push_back(cl);
    }

    option_localization_r find_option_localization(size_t index) const;

    option_localization_r find_option_localization(const option_r&) const;

    void check_more(const string& label, const question_body_r& qb) const override;
    
    // Helpers.

    template <typename SourceQuestion>
    source_question_r tmpl_make_source_question(const question_r& q) const {
      auto qbmc = checked_cast<question_body_multiple_choices>(q->get_body());
      auto rtnd = make<SourceQuestion>(q->get_label(), qbmc->get_style(), make<source_text>(qbmc, *this), get_comment_label(), qbmc->get_randomize(), qbmc->get_limit());
      auto i = options_cbegin();
      auto e = options_cend();
      
      while (i != e){
	option_localization_p if_ol = *i;
	HX2A_ASSERT(if_ol);
	option_localization_r ol = *if_ol;
	rtnd->_options.push_back(make<source_option>(ol->get_label(), ol->get_comment_label()));
	++i;
      }

      q->add_transitions_to(rtnd);
      
      return rtnd;
    }

    void add_options_to(const source_template_question_with_options_r& to) const;

    void add_options_to(const source_template_question_localization_with_options_r& to) const;

  private:

    options_localizations_type _options;
  };

  class question_localization_body_select: public question_localization_body_with_options
  {
    HX2A_ELEMENT(question_localization_body_select, type_tag<"q_l10n_b_select">, question_localization_body_with_options,
		 ());
  public:

    using question_localization_body_with_options::question_localization_body_with_options;

    // The check function on the base class is fine. No need to add anything.
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;

    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_select_at_most: public question_localization_body_with_options
  {
    HX2A_ELEMENT(question_localization_body_select_at_most, type_tag<"q_l10n_b_select_at_most">, question_localization_body_with_options,
		 ());
  public:

    using question_localization_body_with_options::question_localization_body_with_options;

    // The check function on the base class is fine. No need to add anything.
    // No specific function to make the JSON source question localization body.
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;

    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_select_limit: public question_localization_body_with_options
  {
    HX2A_ELEMENT(question_localization_body_select_limit, type_tag<"q_l10n_b_select_limit">, question_localization_body_with_options,
		 ());
  public:

    using question_localization_body_with_options::question_localization_body_with_options;

    // The check function on the base class is fine. No need to add anything.
    // No specific function to make the JSON source question localization body.
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;

    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_rank_at_most: public question_localization_body_with_options
  {
    HX2A_ELEMENT(question_localization_body_rank_at_most, type_tag<"q_l10n_b_rank_at_most">, question_localization_body_with_options,
		 ());
  public:

    using question_localization_body_with_options::question_localization_body_with_options;

    // The check function on the base class is fine. No need to add anything.
    // No specific function to make the JSON source question localization body.
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;

    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  class question_localization_body_rank_limit: public question_localization_body_with_options
  {
    HX2A_ELEMENT(question_localization_body_rank_limit, type_tag<"q_l10n_b_rank_limit">, question_localization_body_with_options,
		 ());
  public:

    using question_localization_body_with_options::question_localization_body_with_options;

    // The check function on the base class is fine. No need to add anything.
    // No specific function to make the JSON source question localization body.
    
    source_question_r make_source_question(const question_r&) const override;
    
    source_template_question_r make_source_template_question(const template_question_localization_r&) const override;

    source_template_question_localization_r make_source_template_question_localization(const template_question_localization_r&) const override;
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack&,
						 language_t,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const override;
  };

  // Answers bear a link towards question localizations, so they need to be anchors.
  // Although we do not need a body on this class, and we could just use inheritance, for sharing the
  // type (not the document) with the type template question localization, we have a body.
  class question_localization: public anchor<>
  {
    HX2A_ANCHOR(question_localization, type_tag<"q_l10n">, anchor,
		(_question, _body));
  public:

    question_localization(const question_r& q, const question_localization_body_r& body):
      _question(*this, q),
      _body(*this, body)
    {
    }
    
    void check() const {
      HX2A_ASSERT(_body);
      HX2A_ASSERT(_question);
      _body->check(_question->get_label(), _question->get_body());
    }

    question_r get_question() const { return *_question; }

    const string& get_label() const { return _question->get_label(); }

    question_localization_body_r get_body() const {
      HX2A_ASSERT(_body);
      return *_body;
    }

    source_question_r make_source_question() const {
      HX2A_ASSERT(_body);
      HX2A_ASSERT(_question);
      return _body->make_source_question(*_question);
    }

    localized_question_r make_localized_question(
						 const the_stack& ts,
						 language_t lang,
						 const string& logo,
						 const string& title,
						 progress_t progress
						 ) const {
      HX2A_ASSERT(_body);
      HX2A_ASSERT(_question);
      return _body->make_localized_question(get_label(), ts, lang, logo, title, *_question, progress);
    }

  private:

    link<question, "q"> _question;
    own<question_localization_body, "b"> _body;
  };

  using questionnaire_localization_map_per_question = std::unordered_map<const question*, question_localization_p>;

  // This type cannot share a common base class with question_localization, as this is a root while the latter is an anchor.
  class template_question_localization: public root<>
  {
    HX2A_ROOT(template_question_localization, type_tag<"tq_l10n">, 1, root,
	      (_template_question, _language, _body));
  public:

    template_question_localization(
				   const template_question_r& tq,
				   language_t lang,
				   const question_localization_body_r& body
				   ):
      _template_question(*this, tq),
      _language(*this, lang),
      _body(*this, body)
    {
      // check must not be called here.
    }

    void check() const {
      HX2A_ASSERT(_body);
      HX2A_ASSERT(_template_question);
      _body->check(_template_question->get_label(), _template_question->get_body());
    }

    template_question_r get_template_question() const {
      HX2A_ASSERT(_template_question);
      return *_template_question;
    }
    
    language_t get_language() const { return _language; }

    void update_language(language_t l){
      if (l != language::nil()){
	// An attempt is made to change the language of the localization. Let's see if one already exists or not
	// before proceeding.
	if (template_question_localization::find(get_template_question(), l)){
	  // One exists, we cannot proceed.
	  throw template_question_localization_already_exists();
	}
	
	// All is good, we update the language.
	_language = l;
      }
    }

    question_localization_body_r get_body() const {
      HX2A_ASSERT(_body);
      return *_body;
    }

    // Returns a source question from template. Not returning this strong type to help the compiler
    // push in lists/vectors and avoid an ambiguity.
    source_question_r make_source_question(const question_from_template_r&) const;

    source_template_question_r make_source_template_question() const {
      HX2A_ASSERT(_body);
      return _body->make_source_template_question(*this);
    }

    source_template_question_localization_r make_source_template_question_localization() const {
      HX2A_ASSERT(_body);
      return _body->make_source_template_question_localization(*this);
    }
    
    localized_question_r make_localized_question(
						 const string& label,
						 const the_stack& ts,
						 language_t lang,
						 const string& logo,
						 const string& title,
						 const question_r& q,
						 progress_t progress
						 ) const {
      HX2A_ASSERT(_body);
      return _body->make_localized_question(label, ts, lang, logo, title, q, progress);
    }

    static template_question_localization_p find(const template_question_r&, language_t);

  private:

    link<template_question, "q"> _template_question;
    // The language is validated at source compilation level.
    slot<language_t, "l"> _language;
    // There is nothing template specific to a template question localization body, we can use
    // a regular one.
    own<question_localization_body, "body"> _body;
  };
  
  // The questions localizations are not in the same order as the questions in the questionnaire.
  // The mapping materializes as links from question localizations to questions.
  // When scanning an entire questionnaire localization, check it first and build one of the maps
  // using the dump function.
  // Reserve the find functions for on the spot searches.
  class questionnaire_localization: public root<>
  {
    HX2A_ROOT(questionnaire_localization, type_tag<"qq_l10n">, 1, root,
	      (_questionnaire, _questionnaire_change_count, _title, _language, _name, _questions_localizations));
  public:

    using questions_localizations_type = own_list<question_localization, "Q">;

    questionnaire_localization(
			       const questionnaire_r& quest,
			       const string& title,
			       language_t lang,
			       const string& name
			       ):
      _questionnaire(*this, quest),
      // The questionnaire is built with a change count of 1 so that the questionnaire localization check
      // happens at least once.
      _questionnaire_change_count(*this, 0),
      _title(*this, title),
      _language(*this, lang),
      _name(*this, name),
      _questions_localizations(*this)
    {
    }

    questionnaire_r get_questionnaire() const { return *_questionnaire; }

    const string& get_title() const { return _title; }

    language_t get_language() const { return _language; }

    const string& get_name() const { return _name; }
    
    bool is_locked() const {
      return _questionnaire->is_locked();
    }

    // Checks whether the localization is complete and not redundant. A questionnaire can still evolve
    // after a localization has been defined. So a localization might be incomplete.
    // Can be called repeatedly without significant performance impact as it verifies that a check was
    // already made with the version of the questionnaire.
    void check();

    void force_check() const;

    // Beware it is caller's responsibility to call check whenever necessary.
    void push_question_localization_back(const question_localization_r& ql){
      _questions_localizations.push_back(ql);
    }

    questions_localizations_type::const_iterator questions_localizations_cbegin() const {
      return _questions_localizations.cbegin();
    }

    questions_localizations_type::const_iterator questions_localizations_cend() const {
      return _questions_localizations.cend();
    }

    // Assumes that check has been called.
    void dump(questionnaire_localization_map_per_question&) const;
    
    // Prefer the map above if scanning the entire questionnaire localization.
    question_localization_p find_question_localization(const question_r& q) const {
      auto i = std::find_if(_questions_localizations.cbegin(), _questions_localizations.cend(),
			    [&](const question_localization_p& if_ql){
			      HX2A_ASSERT(if_ql);
			      question_localization_r ql = *if_ql;
			      return ql->get_question() == q;
			    });
      
      if (i != _questions_localizations.cend()){
	return *i;
      }

      return {};
    }

    // Finds a questionnaire localization, if any.
    // A database connection must have been established prior to calling this function.
    static questionnaire_localization_p find(const questionnaire_r&, language_t);

    // This ignores loops, it is just an estimate of the % progress wrt individual questions.
    progress_t get_progress(const question_r& q) const { return _questionnaire->get_progress(q); }

  private:

    // Strong link, localizations will be removed automatically when a questionnaire is removed.
    link<questionnaire, "q"> _questionnaire;
    slot<unsigned int, "qcc"> _questionnaire_change_count;
    slot<string, "t"> _title;
    // The language is validated at source compilation level.
    slot<language_t, "l"> _language;
    slot<string, "n"> _name;
    questions_localizations_type _questions_localizations;
  };

  // Interview.
  // We have a protection that locks the questionnaire when a campaign on that questionnaire has been created.
  // The localization and the questionnaire are not allowed to be modified.

  // Each choice bears its index. It means that no individual choice removal/addition is allowed.
  class choice: public element<>
  {
    HX2A_ELEMENT(choice, type_tag<"choice">, element,
		 (_option_localization, _index, _comment));
  public:

    choice(
	   const option_localization_r& c,
	   size_t index, // The index in the options, starting at 0.
	   const string& comment
	   ):
      _option_localization(*this, c),
      _index(*this, index),
      _comment(*this, comment)
    {
    }

    option_localization_r get_option_localization() const { return *_option_localization; }

    const string& get_comment() const { return _comment; }

    size_t get_index() const { return _index; }
    
  private:

    link<option_localization, "option_l10n"> _option_localization;
    // The index of the choice among the options, starting at 0.
    slot<size_t, "i"> _index;
    slot<string, "c"> _comment;
  };

  // The type hierarchy of answer body types more or less matches the hierarchy of question body types.
  // Different question body types expect different data entered by the interviewee.
  class answer_body: public element<>
  {
    HX2A_ELEMENT(answer_body, type_tag<"answer_body">, element,
		 ());
  public:

    answer_body()
    {
    }

    // Dummy definition.
    virtual answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const;
    
    // Dummy definition.
    virtual localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const;

  };

  // Contrary to intuition, there is an answer to message, although there is no input to submit. An interstitial message can
  // be added to an interview, just to show a message. The only way to progress is to submit an empty answer. That empty
  // answer's empty body is below.
  class answer_body_message: public answer_body
  {
    HX2A_ELEMENT(answer_body_message, type_tag<"answer_body_message">, answer_body,
		 ());
  public:
    
    answer_body_message()
    {
    }
    
    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
  };

  class answer_body_with_comment: public answer_body
  {
    HX2A_ELEMENT(answer_body_with_comment, type_tag<"answer_body_with_comment">, answer_body,
		 (_comment));
  public:

    answer_body_with_comment(const string& comment):
      _comment(*this, comment)
    {
    }

    const string& get_comment() const { return _comment; }

  private:
    
    slot<string, "c"> _comment; 
  };
  
  class answer_body_input: public answer_body_with_comment
  {
    HX2A_ELEMENT(answer_body_input, type_tag<"answer_body_input">, answer_body_with_comment,
		 (_input));
  public:

    answer_body_input(const string& input, const string& comment):
      answer_body_with_comment(comment),
      _input(*this, input)
    {
    }

    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
    
  private:
    
    slot<string, "i"> _input;
  };

  // Just to share code.
  class answer_body_with_options: public answer_body_with_comment
  {
    HX2A_ELEMENT(answer_body_with_options, type_tag<"answer_body_with_options">, answer_body_with_comment,
		 ());
  public:

    using answer_body_with_comment::answer_body_with_comment;
    
    // Adds options to the argument.
    void shared_add_options_to_localized_answer_data(const question_localization_body_with_options_r&, const localized_answer_data_with_options_r&) const;
  };
  
  class answer_body_select: public answer_body_with_options
  {
    HX2A_ELEMENT(answer_body_select, type_tag<"answer_body_select">, answer_body_with_options,
		 (_choice));
  public:

    answer_body_select(const choice_r& c, const string& comment):
      answer_body_with_options(comment),
      _choice(*this, c)
    {
    }

    choice_r get_choice() const {
      HX2A_ASSERT(_choice);
      return *_choice;
    }

    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
    
  private:
    
    own<choice, "C"> _choice;
  };

  
  class answer_body_multiple_choices: public answer_body_with_options
  {
    HX2A_ELEMENT(answer_body_multiple_choices, type_tag<"answer_body_multiple_choices">, answer_body_with_options,
		 (_choices));
  public:

    using choices_type = own_list<choice, "C">;

    // The selection is built separately.
    answer_body_multiple_choices(const string& comment):
      answer_body_with_options(comment),
      _choices(*this)
    {
    }

    choices_type::const_iterator selection_cbegin() const { return _choices.cbegin(); }

    choices_type::const_iterator selection_cend() const { return _choices.cend(); }

    void push_choice_back(const choice_r& c){ _choices.push_back(c); }

    // Function adding the options to the argument.
    void shared_add_options_to_answer_data(const answer_data_multiple_choices_r&) const;

    // Adds options AND choices to the argument.
    void shared_add_options_to_localized_answer_data(const question_localization_body_with_options_r&, const localized_answer_data_multiple_choices_r&) const;

  private:
    
    choices_type _choices;
  };

  class answer_body_select_at_most: public answer_body_multiple_choices
  {
    HX2A_ELEMENT(answer_body_select_at_most, type_tag<"answer_body_select_at_most">, answer_body_multiple_choices,
		 ());
  public:

    using answer_body_multiple_choices::answer_body_multiple_choices;

    // No need for a specific make json body.
    
    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
  };
  
  class answer_body_select_limit: public answer_body_multiple_choices
  {
    HX2A_ELEMENT(answer_body_select_limit, type_tag<"answer_body_select_limit">, answer_body_multiple_choices,
		 ());
  public:

    using answer_body_multiple_choices::answer_body_multiple_choices;
    
    // No need for a specific make json body.
    
    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
  };
  
  class answer_body_rank_at_most: public answer_body_multiple_choices
  {
    HX2A_ELEMENT(answer_body_rank_at_most, type_tag<"answer_body_rank_at_most">, answer_body_multiple_choices,
		 ());
  public:

    using answer_body_multiple_choices::answer_body_multiple_choices;
    
    // No need for a specific make json body.
    
    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
  };
  
  class answer_body_rank_limit: public answer_body_multiple_choices
  {
    HX2A_ELEMENT(answer_body_rank_limit, type_tag<"answer_body_rank_limit">, answer_body_multiple_choices,
		 ());
  public:

    using answer_body_multiple_choices::answer_body_multiple_choices;
    
    // No need for a specific make json body.
    
    answer_data_r make_answer_data(const answer_r&, time_t start_timestamp) const override;
    
    localized_answer_data_r make_localized_answer_data(const string& label, const the_stack&, language_t, const question_body_r&, const question_localization_body_r&) const override;
  };
  
  // Exactly on of the two question localization or template question localization will be filled up.
  // An answer contains a body which is polymorphic. The body contains the data filled up for the answer.
  // Anchor because loop stack frames point at answers through a link.
  class answer: public anchor<>
  {
    HX2A_ANCHOR(answer, type_tag<"answer">, anchor,
		 (_question_localization, _template_question_localization, _question_from_template, _ip_address, _elapsed, _total_elapsed, _geolocation, _body));
  public:

    answer(const question_localization_r& ql, string_view ip_address, time_t elapsed, time_t total_elapsed, const geolocation_p& geo, const answer_body_r& body):
      _question_localization(*this, ql),
      _template_question_localization(*this),
      _question_from_template(*this),
      _ip_address(*this, ip_address),
      _elapsed(*this, elapsed),
      _total_elapsed(*this, total_elapsed),
      _geolocation(*this, geo),
      _body(*this, body)
    {
    }

    answer(const template_question_localization_r& tql, const question_from_template_r& qft, string_view ip_address, time_t elapsed, time_t total_elapsed, const geolocation_p& geo, const answer_body_r& body):
      _question_localization(*this),
      _template_question_localization(*this, tql),
      _question_from_template(*this, qft),
      _ip_address(*this, ip_address),
      _elapsed(*this, elapsed),
      _total_elapsed(*this, total_elapsed),
      _geolocation(*this, geo),
      _body(*this, body)
    {
    }

    const string& get_label() const { return get_question()->get_label(); }

    // Returns a pair of question localizations, the standard and the template one, one of them only being
    // non-null.
    localizations get_question_localization() const;

    question_localization_body_r get_question_localization_body() const {
      localizations locs = get_question_localization();
      
      return std::visit(overloaded(
				   [](const question_localization_r& l) {
				     return l->get_body();
				   },
				   [](const template_localization& l){
				     return l.localization->get_body();
				   }),
			locs);
    }

    question_r get_question() const {
      if (_question_localization){
	return _question_localization->get_question();
      }

      HX2A_ASSERT(_template_question_localization);
      HX2A_ASSERT(_question_from_template);
      return *_question_from_template;
    }

    const string& get_ip_address() const { return _ip_address; }

    time_t get_elapsed() const { return _elapsed; }

    time_t get_total_elapsed() const { return _total_elapsed; }

    time_t get_timestamp(time_t interview_start_timestamp) const { return interview_start_timestamp + _total_elapsed; }
    
    geolocation_p get_geolocation() const { return _geolocation; }

    answer_body_r get_body() const {
      HX2A_ASSERT(_body);
      return *_body;
    }
    
    answer_data_r make_answer_data(time_t start_timestamp) const {
      HX2A_ASSERT(_body);
      return _body->make_answer_data(*this, start_timestamp);
    }

    // The localized answer data is calculated with the localization the interview was taken in.
    localized_answer_data_r make_localized_answer_data(const the_stack& ts, language_t lang) const {
      HX2A_ASSERT(_body);

      if (_question_localization){
	return _body->make_localized_answer_data(_question_localization->get_label(), ts, lang, _question_localization->get_question()->get_body(), _question_localization->get_body());
      }

      HX2A_ASSERT(_question_from_template);
      HX2A_ASSERT(_template_question_localization);
      return _body->make_localized_answer_data(_question_from_template->get_label(), ts, lang, _question_from_template->get_body(), _template_question_localization->get_body());
    }

    // The localized answer data is calculated with the imposed localization, which might be different from the one the
    // interview was taken in.
    localized_answer_data_r make_localized_answer_data(const the_stack& ts, language_t lang, const question_localization_r& ql) const {
      HX2A_ASSERT(_body);
      
      if (_question_localization){
	return _body->make_localized_answer_data(_question_localization->get_label(), ts, lang, _question_localization->get_question()->get_body(), ql->get_body());
      }
      
      HX2A_ASSERT(_question_from_template);
      HX2A_ASSERT(_template_question_localization);
      return _body->make_localized_answer_data(_question_from_template->get_label(), ts, lang, _question_from_template->get_body(), _template_question_localization->get_body());
    }

  private:

    // Either the first link will be non-null and the two following ones will be null or
    // the first link will be null and the two following ones will be non-null.
    
    link<question_localization, "ql"> _question_localization;
    
    link<template_question_localization, "tql"> _template_question_localization;
    link<question_from_template, "qft"> _question_from_template;

    slot<string, "ip"> _ip_address;
    slot<time_t, "e"> _elapsed;
    slot<time_t, "te"> _total_elapsed;
    // There is a timestamp in the geolocation.
    own<geolocation, "g"> _geolocation;
    
    // Polymorphic. Contains the response data.
    own<answer_body, "b"> _body;
  };

  class entry: public element<>
  {
    HX2A_ELEMENT(entry, type_tag<"entry">, element,
		 ());
  public:

    using loop_type_t = question::loop_type_t;

    entry(){}

    virtual loop_type_t get_loop_type() const {
      return loop_type_t::regular;
    }

    // Will return the question corresponding to the answer or to the begin loop. Will return
    // null in case of end loop.
    // Dummy.
    virtual question_r get_question() const {
      HX2A_ASSERT(false);
      return make<question>("");
    }

    // Returns whether the entry is impacted by a change in the answer.
    virtual bool is_impacted_by(const answer_r&) const {
      return false;
    }
  };

  class entry_answer: public entry
  {
    HX2A_ELEMENT(entry_answer, type_tag<"entry_a">, entry,
		 (_answer));
  public:

    entry_answer(const answer_r& a):
      _answer(*this, a)
    {}

    answer_r get_answer() const {
      HX2A_ASSERT(_answer);
      return *_answer;
    }

    question_r get_question() const override {
      HX2A_ASSERT(_answer);
      return _answer->get_question();
    }

    bool is_impacted_by(const answer_r& a) const override {
      HX2A_ASSERT(_answer);
      return _answer->get_question()->is_impacted_by(a->get_question());
    }

  private:

    own<answer, "a"> _answer;
  };
  
  class entry_begin_loop: public entry
  {
    HX2A_ELEMENT(entry_begin_loop, type_tag<"entry_bl">, entry,
		 (_question_begin_loop, _loop_operand_answer, _index));
  public:

    entry_begin_loop(const question_begin_loop_r& qbl, const answer_r& loop_answer, size_t index):
      _question_begin_loop(*this, qbl),
      _loop_operand_answer(*this, loop_answer),
      _index(*this, index)
    {}

    question_begin_loop_r get_question_begin_loop() const {
      HX2A_ASSERT(_question_begin_loop);
      return *_question_begin_loop;
    }

    answer_p get_loop_operand_answer() const {
      return _loop_operand_answer;
    }

    size_t get_index() const { return _index; }

    loop_type_t get_loop_type() const override {
      return loop_type_t::begin_loop;
    }

    question_r get_question() const override {
      return get_question_begin_loop();
    }
    
    bool is_impacted_by(const answer_r& a) const override {
      HX2A_ASSERT(_loop_operand_answer);
      return _loop_operand_answer == a;
    }

  private:

    link<question_begin_loop, "qbl"> _question_begin_loop;
    // The link to the loop operand answer must be weak, otherwise if we ever erase an answer,
    // the corresponding answer erasure a strong link would remove the entry begin loop too,
    // which is incorrect.
    weak_link<answer, "loa"> _loop_operand_answer;
    slot<size_t, "i"> _index;
  };

  // It could bear a link pointing at its matching begin loop if it becomes handy.
  class entry_end_loop: public entry
  {
    HX2A_ELEMENT(entry_end_loop, type_tag<"entry_el">, entry,
		 (_question_end_loop));
  public:

    entry_end_loop(const question_end_loop_r& qel):
      _question_end_loop(*this, qel)
    {}

    question_end_loop_r get_question_end_loop() const {
      HX2A_ASSERT(_question_end_loop);
      return *_question_end_loop;
    }
    
    loop_type_t get_loop_type() const override {
      return loop_type_t::end_loop;
    }

    question_r get_question() const override {
      return get_question_end_loop();
    }

  private:

    link<question_end_loop, "qel"> _question_end_loop;
  };
  
  class the_stack_frame
  {
  public:
    
    // Indexes all answers by question. Indexing by question pointer is more efficient that by
    // question label.
    // This is for conditions and parametric texts evaluation.
    // Because of loops there might be several answers for the same question. We keep only the
    // last one.
    using answers_by_question_map = std::unordered_map<const question*, answer_p>;

    the_stack_frame(const the_stack& ts, language_t lang, const question_begin_loop_r& qbl, const answer_r& loa):
      _question_begin_loop(qbl),
      _loop_operand_answer(loa),
      _index(0) // Loop index starts at 0. Increases at each begin loop and decreases at each end loop.
    {
      _loop_operand = json_dom::convert(calculate_loop_operand(ts, lang));
      HX2A_ASSERT(qbl->get_operand_question() == loa->get_question());
      // Let's calculate the loop variable and operand size.
      json::value v = get_loop_operand();

      if (const json::array_type* arr = v.if_array()){
	_loop_operand_size = arr->size();
      }
      else{
	_loop_operand_size = 0;
      }
    }
    
    question_begin_loop_r get_question_begin_loop() const { return _question_begin_loop; }
    
    answer_r get_loop_operand_answer() const { return _loop_operand_answer; }

    json::value get_loop_operand() const { return _loop_operand; }

    size_t get_loop_operand_size() const { return _loop_operand_size; }
    
    size_t get_index() const { return _index; }

    size_t increment_index(){
      _loop_variable_value.reset();
      return ++_index;
    }
    
    const string& get_loop_variable_name() const {
      return _question_begin_loop->get_variable();
    }
    
    void replace_answer(const answer_r& a){
      _answers_by_question_map[&a->get_question().get()] = a;
    }
    
    answer_p find_answer(const question_r& q) const {
      if (auto f = _answers_by_question_map.find(&q.get()); f != _answers_by_question_map.cend()){
	return f->second;
      }
      
      return {};
    }
    
    void dump() const {
      HX2A_LOG(trace) << "Question begin loop " << _question_begin_loop->get_label() << " operating on question operand " << _loop_operand_answer->get_question()->get_label();
      HX2A_LOG(trace) << "Index is " << _index;
      
      if (_answers_by_question_map.empty()){
	HX2A_LOG(trace) << "No answers.";
      }
      else{
	HX2A_LOG(trace) << "Answers:";
	
	for (const auto& p: _answers_by_question_map){
	  HX2A_LOG(trace) << p.first->get_label();
	}
      }
    }

    json::value get_loop_variable_value(const the_stack& ts, language_t lang) const {
      if (_loop_variable_value){
	return *_loop_variable_value;
      }
      
      json_dom::value v = calculate_loop_variable_value(ts, lang, _question_begin_loop, _loop_operand_answer, _index);
      _loop_variable_value = json_dom::convert(v);
      return *_loop_variable_value;
    }
    
    static json_dom::value calculate_loop_variable_value(const the_stack&, language_t, const question_begin_loop_r&, const answer_r& loop_operand_answer, size_t index);

    json_dom::value calculate_loop_operand(const the_stack&, language_t) const;
    
  private:

    question_begin_loop_r _question_begin_loop;
    answer_r _loop_operand_answer;
    json::value _loop_operand;
    size_t _loop_operand_size;
    mutable std::optional<json::value> _loop_variable_value;
    size_t _index;
    // All the answers in the loop nest per question pointer. There is guaranteed unicity. The map does not contain
    // answers in higher nest levels, we're using all this one-off so there is no point optimizing much.
    answers_by_question_map _answers_by_question_map;
  };

  // The stack contains a map of answers by question at every stack frame level to be able to pop frames easily when
  // one or several question end loop are encountered.
  class the_stack
  {
  public:

    using answers_by_question_map = the_stack_frame::answers_by_question_map;

    bool empty() const { return _vector.empty(); }
    
    size_t size() const { return _vector.size(); }

    question_begin_loop_r get_question_begin_loop(){
      HX2A_ASSERT(_vector.size());
      return _vector.back().get_question_begin_loop();
    }

    size_t get_index() const {
      HX2A_ASSERT(_vector.size());
      return _vector.back().get_index();
    }

    void process_entry(language_t lang, const entry_r& e){
      switch (e->get_loop_type()){
      case question::regular:
	{
	  entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
	  HX2A_ASSERT(ea);
	  replace_answer(ea->get_answer());
	  break;
	}
	
      case question::begin_loop:
	{
	  HX2A_LOG(trace) << "During stack calculation, encountered a begin loop.";
	  entry_begin_loop* ebl = dynamic_cast<entry_begin_loop*>(&e.get());
	  HX2A_ASSERT(ebl);
	  HX2A_ASSERT(ebl->get_loop_operand_answer());
	  process_begin_loop(lang, ebl->get_question_begin_loop(), *ebl->get_loop_operand_answer());
	  break;
	}
	
      case question::end_loop:
	{
	  HX2A_LOG(trace) << "During stack calculation, encountered an end loop.";
	  // We pop a frame only if the index is 0. Otherwise we decrement the index.
	  HX2A_ASSERT(size());
	  process_end_loop();
	  break;
	}
      }
    }

    void process_begin_loop(language_t lang, const question_begin_loop_r& qbl, const answer_r& loa){
      if (_vector.empty()){
	_vector.emplace_back(the_stack_frame(*this, lang, qbl, loa));
      }
      else{
	the_stack_frame& tsf = _vector.back();

	if (tsf.get_question_begin_loop() != qbl){
	  _vector.emplace_back(*this, lang, qbl, loa);
	}
      }
    }
    
    // Returns the loop operand answer if it was found.
    answer_p process_begin_loop(language_t lang, const question_begin_loop_r& qbl){
      answer_p loa = find_answer(qbl->get_operand_question());
      HX2A_LOG(trace) << "Processing a begin loop. The answer operand is" << (loa ? " non" : "" ) << " null.";
      
      if (!loa){
	return {};
      }

      json_dom::value first = the_stack_frame::calculate_loop_variable_value(*this, lang, qbl, *loa, 0);
      HX2A_LOG(trace) << "The first value of the loop variable is " << first;

      if (!first){
	return {};
      }
      
      process_begin_loop(lang, qbl, *loa);
      return loa;
    }

    // Returns true if no frame popping happened.
    bool process_end_loop(){
      HX2A_ASSERT(_vector.size());
      the_stack_frame& tsf = _vector.back();

      if (tsf.increment_index() == tsf.get_loop_operand_size()){
	HX2A_LOG(trace) << "Popping the stack.";
	// End of loop.
	_vector.pop_back();
	return false;
      }

      HX2A_LOG(trace) << "Not popping the stack.";
      return true;
    }
    
    json::value get_loop_variable(language_t lang, const string& name) const {
      // We search from the innermost nest.
      auto i = _vector.crbegin();
      auto e = _vector.crend();
      
      while (i != e){
	if (i->get_loop_variable_name() == name){
	  return i->get_loop_variable_value(*this, lang);
	}
	
	++i;
      }
      
      return {};
    }

    json::value get_loop_variable_value(language_t lang) const {
      HX2A_ASSERT(_vector.size());
      return _vector.back().get_loop_variable_value(*this, lang);
    }
    
    void replace_answer(const answer_r& a){
      if (_vector.empty()){
	_answers_by_question_map[&a->get_question().get()] = a;
      }
      else{
	_vector.back().replace_answer(a);
      }
    }
    
    answer_p find_answer(const question_r& q) const {
      // We search from the innermost nest.
      auto i = _vector.crbegin();
      auto e = _vector.crend();
      
      while (i != e){
	if (answer_p a = i->find_answer(q)){
	  return a;
	}
	
	++i;
      }
      
      auto f = _answers_by_question_map.find(&q.get());
      
      if (f != _answers_by_question_map.cend()){
	return f->second;
      }
      
      return {};
    }

    answer_p find_loop_operand_answer(const question_begin_loop_r& qbl) const {
      // We search from the innermost nest.
      auto i = _vector.crbegin();
      auto e = _vector.crend();
      
      while (i != e){
	if (i->get_question_begin_loop() == qbl){
	  return i->get_loop_operand_answer();
	}
	
	++i;
      }
      
      return {};
    }
    
    void dump() const {
      if (_answers_by_question_map.empty()){
	HX2A_LOG(trace) << "No answers.";
      }
      else{
	HX2A_LOG(trace) << "Top level answers:";
	
	for (const auto& p: _answers_by_question_map){
	  HX2A_LOG(trace) << p.first->get_label();
	}
      }
      
      if (_vector.empty()){
	HX2A_LOG(trace) << "No stack frames.";
      }
      else{
	size_t i = 0;
	
	for (const auto& sf: _vector){
	  HX2A_LOG(trace) << "Stack frame #" << i;
	  sf.dump();
	}
      }
    }
    
  private:
    
    // Innermost nest last.
    ::std::vector<the_stack_frame> _vector;
    // The map of answers at the top, without a loop nest, per question pointer. There is guaranteed unicity.
    answers_by_question_map _answers_by_question_map;
  };
  
  // The process to start an interview is usually as follows:
  // - A campaign is created on a given questionnaire. Questionnaires are reusable across different campaigns.
  // - Interviews a pre-created for all the interviewees in the campaign. That allows to have a "stock number"/
  //   unique identifier for each interview (its document identifier), which prevents unwanted people to take
  //   an interview. The interview is communicated to the interviewee by some specific means (e.g. email).
  // - An interview is first started. That does not involve asking (as well as answering) any questionnaire. It
  //   requires selecting a language in which the interview will be conducted.
  // - Once an interview is started, successive "next question" calls are made that supply the next question in
  //   the selected language.
  // - Following a question being sent, an answer is submitted and if correct, it is recorded in the interview.
  //   The answer is devoid of any language, except free fields.
  // - Then the process loops to the next question, until there is no next question and the interview is complete.
  // Throughout the process, the interview moves from a state to another, from "initiated" (just created),
  // "started" (it'll stay in this state throughout all questions and answers), until there is no next question
  // and it is marked "complete".
  class interview: public root<>
  {
    HX2A_ROOT(interview, type_tag<"i">, 1.1, root,
	      (_campaign, _start_ip_address, _start_timestamp, _start_geolocation, _interviewee_id, _interviewer_id, _interviewer_user, _language, _questionnaire_localization, _history, _state, _next_question));
    
  public:

    using history_type = own_list<entry, "h">;

    enum state_t {
		  initiated = 0, // Interview just created and not started.
		  ongoing = 1,
		  completed = 2
    };

    // Creating an interview does not check yet whether the campaign is active.
    // That way it is possible to create a bunch of interviews in advance and then once
    // the campaign is active, send a proposal to interviewees to take the interview.
    interview(const campaign_r& campaign):
      _campaign(*this, campaign),
      _start_ip_address(*this),
      _start_timestamp(*this),
      _start_geolocation(*this),
      _interviewee_id(*this),
      _interviewer_id(*this),
      _interviewer_user(*this),
      _language(*this),
      _questionnaire_localization(*this),
      _history(*this),
      _state(*this, initiated),
      _next_question(*this)
    {
    }

    const string& get_start_ip_address() const { return _start_ip_address; }

    time_t get_start_timestamp() const { return _start_timestamp; }

    geolocation_p get_start_geolocation() const { return _start_geolocation; }
    
    campaign_r get_campaign() const { return *_campaign; }

    const string& get_interviewee_id() const { return _interviewee_id; }

    const string& get_interviewer_id() const { return _interviewer_id; }

    user_p get_interviewer_user() const { return _interviewer_user; }

    language_t get_language() const { return _language; }

    // The questionnaire localization is null until the interview starts.
    questionnaire_localization_p get_questionnaire_localization() const { return _questionnaire_localization; }

    questionnaire_r get_questionnaire() const {
      // Once started, the questionnaire could also be obtained through the questionnaire localization.
      HX2A_ASSERT(_campaign);
      return _campaign->get_questionnaire();
    }

    history_type::const_iterator history_cbegin() const { return _history.cbegin(); }

    history_type::const_iterator history_cend() const { return _history.cend(); }

    history_type::iterator history_begin(){ return _history.begin(); }

    history_type::iterator history_end(){ return _history.end(); }

    state_t get_state() const { return _state; }

    void set_state(state_t s){ _state = s; }

    bool is_started() const { return _state != initiated; }
    
    bool is_completed() const { return _state == completed; }

    void set_next_question(const question_r& q){
      if (q->is_final()){
	_state = completed;
      }
      
      _next_question = q;
    }

    // Non-const, it might update the stack. It will also update the next question.
    question_r calculate_new_next_question(the_stack&);

    // Function either returning the question given in argument if it is not a loop, and if it is a loop,
    // scanning further while running transitions and updating the stack.
    question_r calculate_new_next_question(the_stack&, const question_r&);

    // Checks if the campaign is active.
    void check_active() const {
      _campaign->check_active();
    }

    // Like previous but checks also that the interview has been started.
    void check_live() const {
      if (_state == initiated){
	throw interview_is_not_started();
      }

      check_active();
    }
    
    // Checks whether the campaign is active.
    // Sets the next question to the first question.
    void start(
	       const string& interviewee_id,
	       const string& interviewer_id,
	       const user_p& interviewer_user,
	       language_t language,
	       string_view ip_address,
	       const geolocation_p& geolocation
	       );

    question_localization_p find_question_localization(const question_r& q) const {
      if (!_questionnaire_localization){
	throw interview_is_not_started();
      }
      
      return _questionnaire_localization->find_question_localization(q);
    }

    // The next question as stored on the interview supports localization. The localizations are returned by
    // this function.
    localizations next_question_localization() const;

    localized_question_r move_ahead();

    // Returns the localized question corresponding to the next question stored in the interview.
    // Does not run any transition, just takes the next question stored on the interview
    localized_question_r next_localized_question() const {
      the_stack ts;
      calculate(ts);
      ts.dump();
      return next_localized_question(ts);
    }

    // Same with a stack already built. Avoids rebuilding it.
    localized_question_r next_localized_question(the_stack&) const;

    question_r run_transitions(const the_stack&, const question_r&) const;

    // Implicity applies to the next question.
    // Every time an answer is submitted a check is made that the campaign is still active.
    void add_answer(const answer_r& a){
      _history.push_back(make<entry_answer>(a));
    }

    void add_begin_loop(const question_begin_loop_r& qbl, const answer_r& loop_answer, size_t index){
      _history.push_back(make<entry_begin_loop>(qbl, loop_answer, index));
    }

    void add_end_loop(const question_end_loop_r& qel){
      _history.push_back(make<entry_end_loop>(qel));
    }

    void insert_answer(history_type::iterator pos, const answer_r& a){
      _history.insert(pos, make<entry_answer>(a));
    }

    // Just checks that there is really a regular answer at the index specified.
    history_type::iterator find_answer(size_t index){
      if (index >= _history.size()){
	throw answer_index_does_not_exist(index);
      }

      history_type::iterator i = _history.begin();
      ::std::advance(i, index);
      HX2A_ASSERT(*i);
      
      if ((*i)->get_loop_type() != question::regular){
	throw answer_index_does_not_exist(index);
      }

      return i;
    }
    
    // Returns the iterator pointing at a "real" answer in the history (not a begin or end loop), with an
    // index in the history equal or inferior to the index supplied.
    // The index is updated to the actual value found.
    history_type::iterator find_answer_at_most(size_t& index){
      if (index >= _history.size()){
	throw answer_index_does_not_exist(index);
      }

      auto i = _history.begin();
      ::std::advance(i, index);

      do {
	if (!index){
	  // The first entry is necessarily a real answer.
	  HX2A_ASSERT((*i) && (*i)->get_loop_type() == question::regular);
	  return i;
	}

	HX2A_ASSERT(*i);
	entry_r e = **i;

	if (e->get_loop_type() == question::regular){
	  return i;
	}
	
	--index;
	--i;
      } while (true);
    }

    // Same but equal or superior.
    history_type::iterator find_answer_at_least(size_t& index){
      size_t hsz = _history.size();

      // Although we test in the loop below, we cannot call advance illegitimately. So we have to check.
      if (index >= hsz){
	throw answer_index_does_not_exist(index);
      }

      auto i = _history.begin();
      ::std::advance(i, index);
      size_t idx = index;

      do {
	if (index == hsz){
	  throw answer_index_does_not_exist(idx);
	}

	HX2A_ASSERT(*i);
	entry_r e = **i;

	if (e->get_loop_type() == question::regular){
	  return i;
	}
	
	++index;
	++i;
      } while (true);
    }

    // Returns the last answer in the interview.
    answer_p last_answer() const {
      if (!_history.size()){
	return {};
      }

      auto i = _history.crbegin();
      auto e = _history.crend();

      while (i != e){
	HX2A_ASSERT(*i);
	entry_r e = **i;

	if (e->get_loop_type() == question::regular){
	  entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
	  HX2A_ASSERT(ea);
	  return ea->get_answer();
	}
	
	++i;
      }

      HX2A_ASSERT(false);
      return {};
    }
    
    history_type::iterator find_answer(const string& question_label){
      return std::find_if(_history.begin(), _history.end(), [&](const entry_p& e){
	HX2A_ASSERT(e);
	entry_answer* ea = dynamic_cast<entry_answer*>(e.get());
	return ea && ea->get_question()->get_label() == question_label;
      });
    }

    history_type::iterator find_answer(history_type::iterator from, const question_r& q){
      history_type::iterator e = _history.end();

      while (from != e){
	entry_p e = *from;
	HX2A_ASSERT(e);
	entry_answer* ea = dynamic_cast<entry_answer*>(e.get());

	if (ea && ea->get_question() == q){
	  return from;
	}
	
	++from;
      }
      
      return e;
    }

    // Checks if the interview is active.
    // Performs all the necessary cascading resections of the questionnaire and returns the new localized
    // question, if any.
    // Updates the state of the interview if needed.
    localized_question_p revise_answer(history_type::iterator pos, const answer_r& new_answer);

    // The argument gives the index following the answer needed. If the index is 0, the answer returned is the last one
    // recorded in the interview. This makes the transition from a question to a previous answer easier to implement in
    // the client side.
    // The index for consecutive answers is not incremented by 1, as there could be begin/end loops in between. The only
    // assurance is that an index precisely identifies an answer.
    // Returns null if nothing found.
    localized_answer_data_and_more_payload_p get_previous_answer(size_t);

    // The argument gives the index preceding the answer neded.
    // Returns null if nothing found.
    localized_answer_data_and_more_payload_p get_next_answer(size_t);

    // Gives the elapsed time since the last answer recorded in the interview, and the total elapsed time since beginning
    // of the interview.
    // In case there is no answer yet, the timestamp of the start of the interview is used.
    pair<time_t, time_t> calculate_elapsed_times() const;

    question_p get_next_question() const { return _next_question; }

    // Calculates the stack up to the iterator given in argument. The iterator has to be either the end or point at a
    // regular answer.
    void calculate(the_stack&, history_type::const_iterator) const;

    // Calculates the stack for the whole history.
    void calculate(the_stack& ts) const {
      calculate(ts, _history.cend());
    }
    
  private:

    // Returns the first regular question. It cannot be a loop as loops iterate on previous answers, and this is the first
    // question.
    question_r get_first_question(){
      HX2A_ASSERT(!_next_question);
      HX2A_ASSERT(_questionnaire_localization);
      question_p fq = *_questionnaire_localization->get_questionnaire()->questions_cbegin();
      HX2A_ASSERT(fq);
      // The first question cannot be a loop, as loops iterate on previous answers. Nothing to search.
      return *fq;
    }

    question_r process_begin_loop(the_stack&, const question_begin_loop_r&);

    // It manipulates the index in the stack, so it is not const on the stack.
    question_r process_end_loop(the_stack&, const question_end_loop_r&);

    question_end_loop_p find_matching_end_loop(const question_begin_loop_r&) const;

    // Used within the answer revision process. The new answer has been inserted, and it leads to another question,
    // different from the one in the entry the iterator points at. Resection will erase all subsequent history
    // up to the one that matches the question given in argument. The iterator to the corresponding entry is
    // returned. In case the question is not found, the history end iterator is returned.
    void resect(history_type::iterator& pos, const question_r&);
    
    link<campaign, "c"> _campaign;
    slot<string, "sip"> _start_ip_address;
    // There is also a timestamp in the geolocation. The one below is a server-side one, the geolocation one
    // is a client-side one. They can be compared for geolocation spoofing for instance...
    slot<time_t, "sts"> _start_timestamp;
    own<geolocation, "start_geolocation"> _start_geolocation;
    slot<string, "iee"> _interviewee_id;
    slot<string, "ier"> _interviewer_id;
    // Weak because we don't want the interview to be removed when the interviewer is removed.
    weak_link<user, "iu"> _interviewer_user;
    // So that we remember in which language the interview was conducted in.
    // We do not want the interview to be removed if the localization has been removed. So
    // we keep the language, and the link to the localization is weak.
    // It also allows to index easily.
    slot<language_t, "l"> _language;
    // If the localization is removed, the interview is removed. When the interview is not started yet, there
    // is no questionnaire localization. It appears once a language has been selected during start.
    link<questionnaire_localization, "l10n"> _questionnaire_localization;
    // Answers or begin/end loops in succession.
    history_type _history;
    slot<state_t, "s"> _state;
    // Next non loop question. It can be regular or template. Once an interview has been started, this link will
    // never be null. Once the final question is reached, that link will keep pointing at it.
    // When an interviewee comes back to an interview, without that link, we would have to rerun the transitions
    // to find the next question again. As transitions can be random, it would not be reliable.
    // A strong link would be more costly and unnecessary.
    weak_link<question, "n"> _next_question;
  };

  // Inlines.
  
  inline question_body_r question_from_template::get_body() const {
    HX2A_ASSERT(_template_question);
    return _template_question->get_body();
  }

  inline localizations answer::get_question_localization() const {
    if (_question_localization){
      HX2A_ASSERT(!_template_question_localization && !_question_from_template);
      return *_question_localization;
    }
    else{
      HX2A_ASSERT(_template_question_localization && _question_from_template);
      return template_localization{*_template_question_localization, *_question_from_template};
    }
  }

} // End namespace interviews.

#endif
