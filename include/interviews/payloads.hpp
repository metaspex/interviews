//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#ifndef HX2A_INTERVIEWS_PAYLOADS_HPP
#define HX2A_INTERVIEWS_PAYLOADS_HPP

// Edit JavaScript code/conditions in a normal editor with new lines and to use:
// tr -d '\n\r' <file.js >file.oneline.js to paste the contents in questionnaire JSON files.

// General design notes:
//
// We're extensively using payload polymorphism as service query and services replies payloads. This is due to the
// fact that many services are extensively polymorphic, in particular the ones uploading a questionnaire or a
// questionnaire localization.
// Also, instead of having multiple answer services, one for each type of question, we have decided to have a single
// answer service which receives a polymorphic payload.
// Instead of using weakly-typed JSON values to implement payload polymorhpism, we're using the safer strongly-typed
// polymorphism, which is automated on payloads (the {"$type_tag": {...}} form).
// As a result, there are multiple type hierarchies that look very similar in payloads. This would usually be resolved
// by having a more limited set of hierarchies and having various types point at shared types (with a own relationship).
// It would translate into having a "body" for some of the types, that body being the sub-object owned. The need for
// that body is not caused by the fact that independently-lived objects exist (the origin of the relationship and the
// body), but just to share types.
// If we had bodies, this would have the consequence of having more complex payloads be sent by the client, each payload
// having to have that body. So we accept to pay the price for independent type hierarchies that look very similar here.
// In some scarce cases, a body has been used in some of the payloads to avoid multiplying type hierarchies.
// Generic programming has not been used to write type hierarchies once and duplicate them with different type hierarchies
// because there are subtle differences between the hierarchies, and because the functions on the types are different.
// It is not reasonable to share types just because they might have the same data, but actually do not have the same
// functions.
// Special care has been paid at having some type tags very short and user-friendly as they are user-facing (the
// polymorphic JSON payloads contain them).

#include <set>
#include <utility>

#include "hx2a/limit.hpp"
#include "hx2a/element.hpp"
#include "hx2a/slot.hpp"
#include "hx2a/own.hpp"
#include "hx2a/own_vector.hpp"
// For localization.
#include "hx2a/components/language.hpp"

#include "interviews/ontology.hpp"

namespace interviews {

  using namespace hx2a;

  class source_option;
  using source_option_p = ptr<source_option>;
  using source_option_r = rfr<source_option>;
  
  class source_option_localization;
  using source_option_localization_p = ptr<source_option_localization>;
  using source_option_localization_r = rfr<source_option_localization>;

  class source_function;
  using source_function_p = ptr<source_function>;
  using source_function_r = rfr<source_function>;
  
  class source_text;
  using source_text_p = ptr<source_text>;
  using source_text_r = rfr<source_text>;
  
  // Template library payloads.

  class template_question_category_id;
  using template_question_category_id_p = ptr<template_question_category_id>;
  using template_question_category_id_r = rfr<template_question_category_id>;
  
  class template_question_category_id_and_language_payload;
  using template_question_category_id_and_language_payload_p = ptr<template_question_category_id_and_language_payload>;
  using template_question_category_id_and_language_payload_r = rfr<template_question_category_id_and_language_payload>;
  
  class template_question_category_data;
  using template_question_category_data_p = ptr<template_question_category_data>;
  using template_question_category_data_r = rfr<template_question_category_data>;
  
  class template_question_id;
  using template_question_id_p = ptr<template_question_id>;
  using template_question_id_r = rfr<template_question_id>;
  
  class template_question_id_and_language_payload;
  using template_question_id_and_language_payload_p = ptr<template_question_id_and_language_payload>;
  using template_question_id_and_language_payload_r = rfr<template_question_id_and_language_payload>;
  
  class template_question_data;
  using template_question_data_p = ptr<template_question_data>;
  using template_question_data_r = rfr<template_question_data>;
  
  class source_template_question;
  using source_template_question_p = ptr<source_template_question>;
  using source_template_question_r = rfr<source_template_question>;

  class source_template_question_localization;
  using source_template_question_localization_p = ptr<source_template_question_localization>;
  using source_template_question_localization_r = rfr<source_template_question_localization>;

  class template_question_localization_update_payload;
  using template_question_localization_update_payload_p = ptr<template_question_localization_update_payload>;
  using template_question_localization_update_payload_r = rfr<template_question_localization_update_payload>;
  
  // Source questionnaire payloads.
  
  class languages_payload;
  using languages_payload_p = ptr<languages_payload>;
  using languages_payload_r = rfr<languages_payload>;
  
  class source_transition;
  using source_transition_p = ptr<source_transition>;
  using source_transition_r = rfr<source_transition>;

  class source_question;
  using source_question_p = ptr<source_question>;
  using source_question_r = rfr<source_question>;

  class source_question_from_template;
  using source_question_from_template_p = ptr<source_question_from_template>;
  using source_question_from_template_r = rfr<source_question_from_template>;

  class source_question_begin_loop;
  using source_question_begin_loop_p = ptr<source_question_begin_loop>;
  using source_question_begin_loop_r = rfr<source_question_begin_loop>;

  class source_question_end_loop;
  using source_question_end_loop_p = ptr<source_question_end_loop>;
  using source_question_end_loop_r = rfr<source_question_end_loop>;

  class source_questionnaire;
  using source_questionnaire_p = ptr<source_questionnaire>;
  using source_questionnaire_r = rfr<source_questionnaire>;

  class source_question_localization;
  using source_question_localization_p = ptr<source_question_localization>;
  using source_question_localization_r = rfr<source_question_localization>;

  class source_questionnaire_localization;
  using source_questionnaire_localization_p = ptr<source_questionnaire_localization>;
  using source_questionnaire_localization_r = rfr<source_questionnaire_localization>;

  // Others.

  class interview_id_payload;
  using interview_id_payload_p = ptr<interview_id_payload>;
  using interview_id_payload_r = rfr<interview_id_payload>;

  class interview_data;
  using interview_data_p = ptr<interview_data>;
  using interview_data_r = rfr<interview_data>;

  class campaign_data;
  using campaign_data_p = ptr<campaign_data>;
  using campaign_data_r = rfr<campaign_data>;

  class localized_question;
  using localized_question_p = ptr<localized_question>;
  using localized_question_r = rfr<localized_question>;

  class answer_payload;
  using answer_payload_p = ptr<answer_payload>;
  using answer_payload_r = rfr<answer_payload>;

  class choice_payload;
  using choice_payload_p = ptr<choice_payload>;
  using choice_payload_r = rfr<choice_payload>;
  
  class answer_data;
  using answer_data_p = ptr<answer_data>;
  using answer_data_r = rfr<answer_data>;

  class localized_answer_data;
  using localized_answer_data_p = ptr<localized_answer_data>;
  using localized_answer_data_r = rfr<localized_answer_data>;

  class localized_answer_data_and_more_payload;
  using localized_answer_data_and_more_payload_p = ptr<localized_answer_data_and_more_payload>;
  using localized_answer_data_and_more_payload_r = rfr<localized_answer_data_and_more_payload>;

  class questionnaire_and_localization_ids;
  using questionnaire_and_localization_ids_p = ptr<questionnaire_and_localization_ids>;

  class localized_interview_data;
  using localized_interview_data_p = ptr<localized_interview_data>;

  class submit_answer_payload;
  using submit_answer_payload_p = ptr<submit_answer_payload>;
  using submit_answer_payload_r = rfr<submit_answer_payload>;

  class source_option: public element<>
  {
    HX2A_ELEMENT(source_option, type_tag<"src_option">, element,
		 ((_label, label_tag),
		  (_comment_label, comment_label_tag)));
  public:

    source_option(
		  const string& label,
		  const string& comment_label
		  ):
      _label(*this, label),
      _comment_label(*this, comment_label)
    {
    }

    source_option(const option_localization_r& ol):
      _label(*this, ol->get_label()),
      _comment_label(*this, ol->get_comment_label())
    {
    }
  
    slot<string> _label;
    slot<string> _comment_label;
  };

  class source_option_localization: public element<>
  {
    HX2A_ELEMENT(source_option_localization, type_tag<"src_option_l10n">, element,
		 ((_label, label_tag),
		  (_comment_label, comment_label_tag)));
  public:

    source_option_localization(
			       const string& label,
			       const string& comment_label
			       ):
      _label(*this, label),
      _comment_label(*this, comment_label)
    {
    }

    source_option_localization(const option_localization_r& ol):
      _label(*this, ol->get_label()),
      _comment_label(*this, ol->get_comment_label())
    {
    }
  
    slot<string> _label;
    slot<string> _comment_label;
  };

  class source_function: public element<>
  {
    HX2A_ELEMENT(source_function, type_tag<"src_function">, element,
		 ((_parameters, parameters_tag),
		  (_code, code_tag)));
  public:

    source_function(const string& code):
      _parameters(*this),
      _code(*this, code)
    {
    }

    // The question number is used to check if parameter questions are before, on the question or after.
    // Label given only for error reporting purposes.
    void compile(const string& label, const question_body_r& qb, const question_infos_by_label_map& qblm, const question_info&) const;
    
    slot_vector<string> _parameters;
    slot<string> _code;
  };
  
  class source_text: public element<>
  {
    HX2A_ELEMENT(source_text, type_tag<"src_text">, element,
		 ((_functions, functions_tag),
		  (_value, value_tag)));
  public:

    source_text(const question_body_r& qb, const question_localization_body_r& qlb):
      _functions(*this),
      _value(*this, qlb->get_text())
    {
      auto i = qb->text_functions_cbegin();
      auto e = qb->text_functions_cend();

      while (i != e){
	HX2A_ASSERT(*i);
	function_r f = **i;
	source_function_r sf = make<source_function>(f->get_code());
	auto pi = f->parameters_cbegin();
	auto pe = f->parameters_cend();

	while (pi != pe){
	  HX2A_ASSERT(*pi);
	  sf->_parameters.push_back((*pi)->get_label());
	  ++pi;
	}
	
	_functions.push_back(sf);
	++i;
      }
    }

    // The question number is used to check if parameter questions are before, on the question or after.
    // Label given only for error reporting purposes.
    void compile(const string& label, const question_body_r& qb, const question_infos_by_label_map& qblm, const question_info&) const;

    // A given text can use multiple functions.
    own_vector<source_function> _functions;
    slot<string> _value;
  };

  // Source template questions. They have the same taxonomy as source questions, but each of them has a language.
  // The language is specific per template source question, while it is common to all source questions in a
  // questionnaire, the questionnaire bearing the common language.

  // The label of the template question is the unique label to refer to it. Nothing to do with question labels in
  // a questionnaire.
  class source_template_question: public element<>
  {
    HX2A_ELEMENT(source_template_question, type_tag<"src_template_question">, element,
		 ((_language, language_tag),
		  (_label, label_tag),
		  (_style, style_tag),
		  (_text, text_tag)));
  public:

    source_template_question(serial_t):
      element(serial),
      _language(*this, language::nil()), // It defaults to nil so that we can identify that no language was given.
      _label(*this),
      _style(*this), // Unlike with regular questions, the style can be brought up here.
      _text(*this)
    {
    }

    source_template_question(
			     language_t language,
			     const string& label,
			     const string& style,
			     const string& text
			     ):
      _language(*this, language),
      _label(*this, label),
      _style(*this, style),
      _text(*this, text)
    {
    }
    
    source_template_question(const template_question_localization_r&);

    // This will create the template question document, if successful. It will be created in the same database as the
    // template question category.
    // This will also create the template question localization document, if any. Same for the database.
    // This is a one pass algorithm. It creates the template question localization and the template question at the same
    // time. That little saving creates the situation whereby the update function repeats some of the code.
    // Dummy base implementation.
    // Non const so that specialized compile functions can do whatever magic they want.
    virtual template_question_r compile(const template_question_category_r& tqc){
      HX2A_ASSERT(false);
      return make<template_question>(*tqc->get_home(), tqc, "", make<question_body>(""));
    }

    // The template question can be retrieved from its localization.
    // Both template question and template question localization will be updated.
    // Dummy base implementation.
    virtual void update(const template_question_localization_r&) const {
      HX2A_ASSERT(false);
    }

    slot<language_t> _language;
    slot<string > _label;
    slot<string> _style;
    // Every localization in a source question will need the text of the question/message.
    slot<string> _text;
  };

  // Specializations of source_template_question.
  
  class source_template_question_message: public source_template_question
  {
    HX2A_ELEMENT(source_template_question_message, type_tag<"template_question_message">, source_template_question,
		 ());
  public:

    using source_template_question::source_template_question;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;
  };
  
  class source_template_question_with_comment: public source_template_question
  {
    HX2A_ELEMENT(source_template_question_with_comment, type_tag<"template_question_with_comment">, source_template_question,
		 ((_comment_label, comment_label_tag)));
  public:

    source_template_question_with_comment(
					  language_t language,
					  const string& label,
					  const string& style,
					  const string& text,
					  const string& comment_label
					  ):
      source_template_question(language, label, style, text),
      _comment_label(*this, comment_label)
    {
    }
    
    slot<string> _comment_label;
  };

  // A question with an input field, and an optional comment with another input field.
  class source_template_question_input: public source_template_question_with_comment
  {
    HX2A_ELEMENT(source_template_question_input, type_tag<"template_question_input">, source_template_question_with_comment,
		 ((_optional, optional_tag)));
  public:

    source_template_question_input(serial_t):
      source_template_question_with_comment(serial),
      _optional(*this, false) // Defaulting to mandatory.
    {
    }

    source_template_question_input(
				   language_t language,
				   const string& label,
				   const string& style,
				   const string& text,
				   const string& comment_label,
				   bool optional
				   ):
      source_template_question_with_comment(language, label, style, text, comment_label),
      _optional(*this, optional)
    {
    }
    
    template_question_r compile(const template_question_category_r&) override;

    void update(const template_question_localization_r&) const override;

    slot<bool> _optional;
  };

  // No incrementality is to be expected for updates of classes derived from this type. To simplify the code, options
  // are cleared and rebuilt.
  class source_template_question_with_options: public source_template_question_with_comment
  {
    HX2A_ELEMENT(source_template_question_with_options, type_tag<"template_question_with_options">, source_template_question_with_comment,
		 ((_options, options_tag),
		  (_randomize, randomize_tag)));
  public:

    source_template_question_with_options(
					  language_t language,
					  const string& label,
					  const string& style,
					  const string& text,
					  const string& comment_label,
					  bool randomize
					  ):
      source_template_question_with_comment(language, label, style, text, comment_label),
      _options(*this),
      _randomize(*this, randomize)
    {
    }

    // None of the virtuals are implemented here, this is an intermediary parent type without instances.
    // Shared helpers for derived classes are here, though.
    void compile_options(const question_body_with_options_r& b, const question_localization_body_with_options_r& ql) const {
      ql->options_clear();
      
      for (const auto& sch: _options){
	if (!sch){
	  throw source_question_contains_null_option(_label);
	}
      
	option_r ch = make<option>(sch->_comment_label.get().size());
	b->push_option_back(ch);
	ql->push_option_back(make<option_localization>(_label, ch, sch->_label, sch->_comment_label));
      }
    }

    own_vector<source_option> _options;
    // Rather than multiplying the types or making a template type, we have a boolean driving whether options should be
    // displayed randomized or not by the GUI.
    slot<bool> _randomize;
  };

  class source_template_question_select: public source_template_question_with_options
  {
    HX2A_ELEMENT(source_template_question_select, type_tag<"template_question_select">, source_template_question_with_options,
		 ());
  public:

    using source_template_question_with_options::source_template_question_with_options;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;

    // No specific data.
  };
  
  class source_template_question_multiple_choices: public source_template_question_with_options
  {
    HX2A_ELEMENT(source_template_question_multiple_choices, type_tag<"template_question_multiple_choices">, source_template_question_with_options,
		 ((_limit, limit_tag)));
  public:

    source_template_question_multiple_choices(
					      language_t language,
					      const string& label,
					      const string& style,
					      const string& text,
					      const string& comment_label,
					      bool randomize,
					      size_t limit
					      ):
      source_template_question_with_options(language, label, style, text, comment_label, randomize),
      _limit(*this, limit)
    {
    }

    // None of the virtuals are implemented here, this is an intermediary parent type without instances.
    // Shared helpers for derived classes are here, though.
    
    template <typename QuestionBodyWithOptions, typename QuestionLocalizationBodyWithOptions>
    template_question_r tmpl_compile(const template_question_category_r& tqc){
      adjust_limit(); // If the limit is 0, it becomes the size of the options.
      rfr<QuestionBodyWithOptions> qbwo = make<QuestionBodyWithOptions>(_style, _randomize, _comment_label.get().size(), _limit);
      template_question_r tq = make<template_question>(*tqc->get_home(), tqc, _label, qbwo);
      rfr<QuestionLocalizationBodyWithOptions> qlbwo = make<QuestionLocalizationBodyWithOptions>(_text, _comment_label);
      make<template_question_localization>(*tqc->get_home(), tq, _language, qlbwo);
      // Now let's take care of the options for both in a single shot.
      compile_options(qbwo, qlbwo);
      return tq;
    }

    // In case the limit is null (or not supplied), it is adjusted to the number of options.
    void adjust_limit(){
      if (!_limit){
	_limit = _options.size();
      }
    }
    
    slot<size_t> _limit;
  };
  
  class source_template_question_select_at_most: public source_template_question_multiple_choices
  {
    HX2A_ELEMENT(source_template_question_select_at_most, type_tag<"template_question_select_at_most">, source_template_question_multiple_choices,
		 ());
  public:

    using source_template_question_multiple_choices::source_template_question_multiple_choices;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;

    // No specific data.
  };
  
  class source_template_question_select_limit: public source_template_question_multiple_choices
  {
    HX2A_ELEMENT(source_template_question_select_limit, type_tag<"template_question_select_limit">, source_template_question_multiple_choices,
		 ());
  public:

    using source_template_question_multiple_choices::source_template_question_multiple_choices;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;

    // No specific data.
  };
  
  class source_template_question_rank_at_most: public source_template_question_multiple_choices
  {
    HX2A_ELEMENT(source_template_question_rank_at_most, type_tag<"template_question_rank_at_most">, source_template_question_multiple_choices,
		 ());
  public:

    using source_template_question_multiple_choices::source_template_question_multiple_choices;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;

    // No specific data.
  };
  
  class source_template_question_rank_limit: public source_template_question_multiple_choices
  {
    HX2A_ELEMENT(source_template_question_rank_limit, type_tag<"template_question_rank_limit">, source_template_question_multiple_choices,
		 ());
  public:

    using source_template_question_multiple_choices::source_template_question_multiple_choices;

    template_question_r compile(const template_question_category_r&) override;

    // The template question can be retrieved from its localization.
    void update(const template_question_localization_r&) const override;

    // No specific data.
  };
  
  // End of specializations of source_template_question.
  
  class template_question_category_id: public element<>
  {
    HX2A_ELEMENT(template_question_category_id, type_tag<"template_question_category_id">, element,
		 ((_template_question_category_id, template_question_category_id_tag)));
  public:

    // Strong type for code verification.
    template_question_category_id(const template_question_category_r& tqc):
      _template_question_category_id(*this, tqc->get_id())
    {
    }
    
    slot<doc_id> _template_question_category_id;
  };
  
  class template_question_category_id_and_language_payload: public template_question_category_id
  {
    HX2A_ELEMENT(template_question_category_id_and_language_payload, type_tag<"template_question_category_id_and_language_pld">, template_question_category_id,
		 ((_language, language_tag)));
  public:

    template_question_category_id_and_language_payload(serial_t):
      template_question_category_id(serial),
      _language(*this, language::nil()) // It defaults to nil so that we can identify that no language was given.
    {
    }

    slot<language_t> _language;
  };
  
  class template_question_category_data: public element<>
  {
    HX2A_ELEMENT(template_question_category_data, type_tag<"template_question_category_data_pld">, element,
		 ((_name, name_tag),
		  (_parent, parent_tag)));
  public:

    template_question_category_data(const template_question_category_r& tqc):
      _name(*this, tqc->get_name()),
      _parent(*this)
    {
      if (template_question_category_p par = tqc->get_parent()){
	_parent = (*par)->get_id();
      }
    }

    slot<string> _name;
    slot<doc_id> _parent;
  };

  using template_question_category_create_payload = template_question_category_data;
  using template_question_category_create_payload_p = ptr<template_question_category_create_payload>;
  using template_question_category_create_payload_r = rfr<template_question_category_create_payload>;

  class template_question_category_update_payload: public template_question_category_id
  {
    HX2A_ELEMENT(template_question_category_update_payload, type_tag<"template_question_category_update_pld">, template_question_category_id,
		 ((_name, name_tag)));
  public:

    slot<string> _name;
  };

  class template_question_localization_id: public element<>
  {
    HX2A_ELEMENT(template_question_localization_id, type_tag<"template_question_localization_id">, element,
		 ((_template_question_localization_id, template_question_localization_id_tag)));
  public:

    // Strong type for code verification.
    template_question_localization_id(const template_question_localization_r& tql):
      element(reserved),
      _template_question_localization_id(*this, tql->get_id())
    {
    }

    slot<doc_id> _template_question_localization_id;
  };
  
  class template_question_id: public element<>
  {
    HX2A_ELEMENT(template_question_id, type_tag<"template_question_id">, element,
		 ((_template_question_id, template_question_id_tag)));
  public:

    // Strong type for code verification.
    template_question_id(const template_question_r& tq):
      _template_question_id(*this, tq->get_id())
    {
    }
    
    slot<doc_id> _template_question_id;
  };
  
  class template_question_data: public template_question_category_id
  {
    HX2A_ELEMENT(template_question_data, type_tag<"template_question_data_pld">, template_question_category_id,
		 ((_source_question, question_tag)));
  public:

    template_question_data(const template_question_localization_r& tql):
      template_question_category_id(tql->get_template_question()->get_category()),
      _source_question(*this, make<source_template_question>(tql))
    {
    }

    own<source_template_question> _source_question;
  };

  using template_question_create_payload = template_question_data;
  using template_question_create_payload_p = ptr<template_question_create_payload>;
  using template_question_create_payload_r = rfr<template_question_create_payload>;

  class template_question_get_payload: public element<>
  {
    HX2A_ELEMENT(template_question_get_payload, type_tag<"template_question_get_pld">, element,
		 ((_template_question_localization_id, template_question_localization_id_tag)));
  public:

    slot<doc_id> _template_question_localization_id;
  };

  class template_question_get_from_language_payload: public template_question_id
  {
    HX2A_ELEMENT(template_question_get_from_language_payload, type_tag<"template_question_get_from_language_pld">, template_question_id,
		 ((_language, language_tag)));
  public:

    template_question_get_from_language_payload(serial_t):
      template_question_id(serial),
      _language(*this, language::nil()) // It defaults to nil so that we can identify that no language was given.
    {
    }

    slot<language_t> _language;
  };

  class template_question_update_payload: public template_question_localization_id
  {
    HX2A_ELEMENT(template_question_update_payload, type_tag<"template_question_update_pld">, template_question_localization_id,
		 ((_template_question_category_id, template_question_category_id_tag),
		  (_source_template_question, question_tag)));
  public:

    slot<doc_id> _template_question_category_id;
    own<source_template_question> _source_template_question;
  };

  // We inherit from the template question id a source template question localization needs to refer to the appropriate
  // template question it pertains to.
  class source_template_question_localization: public template_question_id
  {
    HX2A_ELEMENT(source_template_question_localization, type_tag<"source_template_question_localization">, template_question_id,
		 ((_language, language_tag),
		  (_text, text_tag)));
  public:

    source_template_question_localization(serial_t):
      template_question_id(serial),
      _language(*this, language::nil()),  // It defaults to nil so that we can identify that no language was given.
      _text(*this)
    {
    }

    source_template_question_localization(
					  const template_question_r& tq,
					  language_t language,
					  const string& text
					  ):
      template_question_id(tq),
      _language(*this, language),
      _text(*this, text)
    {
    }

    template_question_localization_r compile() const;

    // Dummy implementation.
    virtual template_question_localization_r compile_supplemental(const db::connector& c, const template_question_r& tq, language_t lang) const {
      HX2A_ASSERT(false);
      return make<template_question_localization>(c, tq, lang, make<question_localization_body>(""));
    }
    
    slot<language_t> _language;
    slot<string> _text;
  };

  // Specializations of source_template_question_localization.

  class source_template_question_localization_message: public source_template_question_localization
  {
    HX2A_ELEMENT(source_template_question_localization_message, type_tag<"source_template_question_localization_message">, source_template_question_localization,
		 ());
  public:

    using source_template_question_localization::source_template_question_localization;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };

  // The comment is optional.
  class source_template_question_localization_with_comment: public source_template_question_localization
  {
    HX2A_ELEMENT(source_template_question_localization_with_comment, type_tag<"source_template_question_localization_with_comment">, source_template_question_localization,
		 ((_comment_label, comment_label_tag)));
  public:
    
    source_template_question_localization_with_comment(
						       const template_question_r& tq,
						       language_t language,
						       const string& text,
						       const string& comment_label
						       ):
      source_template_question_localization(tq, language, text),
      _comment_label(*this, comment_label)
    {
    }

    slot<string> _comment_label;
  };
  
  class source_template_question_localization_input: public source_template_question_localization_with_comment
  {
    HX2A_ELEMENT(source_template_question_localization_input, type_tag<"source_template_question_localization_input">, source_template_question_localization_with_comment,
		 ());
  public:

    using source_template_question_localization_with_comment::source_template_question_localization_with_comment;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };
  
  class source_template_question_localization_with_options: public source_template_question_localization_with_comment
  {
    HX2A_ELEMENT(source_template_question_localization_with_options, type_tag<"source_template_question_localization_with_options">, source_template_question_localization_with_comment,
		 ((_options, options_tag)));
  public:
    
    source_template_question_localization_with_options(
						       const template_question_r& tq,
						       language_t language,
						       const string& text,
						       const string& comment_label
						       ):
      source_template_question_localization_with_comment(tq, language, text, comment_label),
      _options(*this)
    {
    }

    // None of the virtuals are implemented here, this is an intermediary parent type without instances.
    // Shared helpers for derived classes are here, though.
    void compile_options(const question_localization_body_with_options_r& qlb, const template_question_r& tq) const {
      auto qbwo = checked_cast<question_body_with_options>(tq->get_body());
      
      if (_options.size() != qbwo->get_options_size()){
	throw template_question_localization_options_size_is_incorrect();
      }
      
      auto i = qbwo->options_cbegin();
    
      for (const auto& sch: _options){
	if (!sch){
	  throw source_question_contains_null_option(tq->get_label());
	}
      
	qlb->push_option_back(make<option_localization>(tq->get_label(), **i, sch->_label, sch->_comment_label));
	++i;
      }
    }

    own_vector<source_option_localization> _options;
  };
  
  class source_template_question_localization_select: public source_template_question_localization_with_options
  {
    HX2A_ELEMENT(source_template_question_localization_select, type_tag<"source_template_question_localization_select">, source_template_question_localization_with_options,
		 ());
  public:

    using source_template_question_localization_with_options::source_template_question_localization_with_options;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };

  class source_template_question_localization_select_at_most: public source_template_question_localization_with_options
  {
    HX2A_ELEMENT(source_template_question_localization_select_at_most, type_tag<"source_template_question_localization_select_at_most">, source_template_question_localization_with_options,
		 ());
  public:

    using source_template_question_localization_with_options::source_template_question_localization_with_options;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };
  
  class source_template_question_localization_select_limit: public source_template_question_localization_with_options
  {
    HX2A_ELEMENT(source_template_question_localization_select_limit, type_tag<"source_template_question_localization_select_limit">, source_template_question_localization_with_options,
		 ());
  public:

    using source_template_question_localization_with_options::source_template_question_localization_with_options;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };
  
  class source_template_question_localization_rank_at_most: public source_template_question_localization_with_options
  {
    HX2A_ELEMENT(source_template_question_localization_rank_at_most, type_tag<"source_template_question_localization_rank_at_most">, source_template_question_localization_with_options,
		 ());
  public:

    using source_template_question_localization_with_options::source_template_question_localization_with_options;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };
  
  class source_template_question_localization_rank_limit: public source_template_question_localization_with_options
  {
    HX2A_ELEMENT(source_template_question_localization_rank_limit, type_tag<"source_template_question_localization_rank_limit">, source_template_question_localization_with_options,
		 ());
  public:

    using source_template_question_localization_with_options::source_template_question_localization_with_options;

    template_question_localization_r compile_supplemental(const db::connector&, const template_question_r&, language_t) const override;
  };
  
  // End of specializations of source_template_question_localization.

  using template_question_localization_create_payload = source_template_question_localization;
  using template_question_localization_create_payload_p = ptr<template_question_localization_create_payload>;
  using template_question_localization_create_payload_r = rfr<template_question_localization_create_payload>;

  class template_question_localization_update_payload: public template_question_localization_id
  {
    HX2A_ELEMENT(template_question_localization_update_payload, type_tag<"template_question_localization_update_pld">, template_question_localization_id,
		 ((_language, language_tag),
		  (_style, style_tag),
		  (_text, text_tag)));
  public:

    template_question_localization_update_payload(serial_t):
      template_question_localization_id(serial),
      _language(*this, language::nil()), // It defaults to nil so that we can identify that no language was given.
      _style(*this),
      _text(*this)
    {
    }

    void update() const;

    // Called by the former.
    virtual void update_supplemental(const question_localization_body_r&, const template_question_r&) const {}
    
    slot<language_t> _language;
    slot<string> _style;
    slot<string> _text;
  };

  // Specializations of template_question_localization_update_payload.

  class template_question_localization_message_update_payload: public template_question_localization_update_payload
  {
    HX2A_ELEMENT(template_question_localization_message_update_payload, type_tag<"template_question_localization_message_update_pld">, template_question_localization_update_payload,
		 ());
  public:

    // No specific update_supplemental.
  };

  class template_question_localization_with_comment_update_payload: public template_question_localization_update_payload
  {
    HX2A_ELEMENT(template_question_localization_with_comment_update_payload, type_tag<"template_question_localization_with_comment_update_pld">, template_question_localization_update_payload,
		 ((_comment_label, comment_label_tag)));
  public:

    void update_supplemental(const question_localization_body_r&, const template_question_r&) const override;
    
    slot<string> _comment_label;
  };

  class template_question_localization_input_update_payload: public template_question_localization_with_comment_update_payload
  {
    HX2A_ELEMENT(template_question_localization_input_update_payload, type_tag<"template_question_localization_input_update_pld">, template_question_localization_with_comment_update_payload,
		 ());
  public:

    // No specific update_supplemental.
  };

  class template_question_localization_with_options_update_payload: public template_question_localization_with_comment_update_payload
  {
    HX2A_ELEMENT(template_question_localization_with_options_update_payload, type_tag<"template_question_localization_with_options_update_pld">, template_question_localization_with_comment_update_payload,
		 ((_options, options_tag)));
  public:

    void update_supplemental(const question_localization_body_r& qlb, const template_question_r& tq) const override;
    
    own_vector<source_option> _options;
  };

  class template_question_localization_select_update_payload: public template_question_localization_with_options_update_payload
  {
    HX2A_ELEMENT(template_question_localization_select_update_payload, type_tag<"template_question_localization_select_update_pld">, template_question_localization_with_options_update_payload,
		 ());
  };

  class template_question_localization_select_at_most_update_payload: public template_question_localization_with_options_update_payload
  {
    HX2A_ELEMENT(template_question_localization_select_at_most_update_payload, type_tag<"template_question_localization_select_at_most_update_pld">, template_question_localization_with_options_update_payload,
		 ());
  };

  class template_question_localization_select_limit_update_payload: public template_question_localization_with_options_update_payload
  {
    HX2A_ELEMENT(template_question_localization_select_limit_update_payload, type_tag<"template_question_localization_select_limit_update_pld">, template_question_localization_with_options_update_payload,
		 ());
  };

  class template_question_localization_rank_at_most_update_payload: public template_question_localization_with_options_update_payload
  {
    HX2A_ELEMENT(template_question_localization_rank_at_most_update_payload, type_tag<"template_question_localization_rank_at_most_update_pld">, template_question_localization_with_options_update_payload,
		 ());
  };

  class template_question_localization_rank_limit_update_payload: public template_question_localization_with_options_update_payload
  {
    HX2A_ELEMENT(template_question_localization_rank_limit_update_payload, type_tag<"template_question_localization_rank_limit_update_pld">, template_question_localization_with_options_update_payload,
		 ());
  };

  // End of specializations of template_question_localization_update_payload.
  
  // Questionnaire payloads.
  
  // Differs from query_id only on the tag.
  class questionnaire_id: public element<>
  {
    HX2A_ELEMENT(questionnaire_id, type_tag<"questionnaire_id">, element,
		 ((_questionnaire_id, questionnaire_id_tag)));
  public:

    slot<doc_id> _questionnaire_id;
  };

  class questionnaire_id_and_language_payload: public questionnaire_id
  {
    HX2A_ELEMENT(questionnaire_id_and_language_payload, type_tag<"questionnaire_id_and_language_pld">, questionnaire_id,
		 ((_language, language_tag)));
  public:

    questionnaire_id_and_language_payload(serial_t):
      questionnaire_id(serial),
      _language(*this, language::nil()) // It defaults to nil so that we can identify that no language was given.
    {
    }

    slot<language_t> _language;
  };

  class logo_payload: public element<>
  {
    HX2A_ELEMENT(logo_payload, type_tag<"header_pld">, element,
		 ((_logo, logo_tag)));
  public:

    logo_payload(serial_t):
      element(serial),
      _logo(*this)
    {
    }
    
    logo_payload(const string& logo):
      _logo(*this, logo)
    {
    }

    slot<string> _logo;
  };
  
  class header_payload: public logo_payload
  {
    HX2A_ELEMENT(header_payload, type_tag<"logo_pld">, logo_payload,
		 ((_title, title_tag)));
  public:

    header_payload(serial_t):
      logo_payload(serial),
      _title(*this)
    {
    }
    
    header_payload(const string& logo, const string& title):
      logo_payload(logo),
      _title(*this, title)
    {
    }

    slot<string> _title;
  };
  
  class languages_payload: public logo_payload
  {
    HX2A_ELEMENT(languages_payload, type_tag<"languages">, logo_payload,
		 ((_languages, languages_tag)));
  public:

    languages_payload(const string& logo):
      logo_payload(logo),
      _languages(*this)
    {
    }

    slot_vector<language_t> _languages;
  };
  
  // Source questionnaire.
  // It contains also a first localization. The corresponding language is on the source questionnaire.
  
  class source_transition: public element<>
  {
    HX2A_ELEMENT(source_transition, type_tag<"src_transition">, element,
		 ((_parameters, parameters_tag),
		  (_condition, condition_tag),
		  (_code, code_tag),
		  (_destination, destination_tag)));
  public:

    source_transition(const transition_r& t):
      _parameters(*this),
      _condition(*this), // We have no way to "reconstruct" the condition, we turned it into code.
      _code(*this, t->get_condition_code()), // On the ontology the code is called a "condition". A little confusing...
      _destination(*this, t->get_destination()->get_label())
    {
      auto i = t->parameters_cbegin();
      auto e = t->parameters_cend();

      while (i != e){
	HX2A_ASSERT(*i);
	_parameters.push_back((*i)->get_label());
	++i;
      }
    }

    slot_vector<string> _parameters;
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
    // the choice is 2.
    slot<string> _condition;
    // In case the snippet is a whole JavaScript piece of code, finishing with the evaluation
    // of the condition.
    slot<string> _code;
    // This is the label of the destination question. It'll be compiled into a link
    // in the compiled questionnaire, if everything is validated.
    slot<string> _destination;
  };

  // A source question is what is submitted to created a new questionnaire.
  // Source questions differ from ontology questions because they contain labels, not links. They are
  // turned into ontology questions which bear links. Also a source question contains a first localization,
  // which ontology questions do not.
  // The functions that turn a source question into a question are the compile functions.
  // The ctors of the source question taking a localization in argument are the inverse functions.
  // These two sets of functions are not necessarily purely inverse from one another, but the result of
  // a download can be submitted back and it will yield the same questionnaire as the initial one.
  // The first question needs a label, although no transition will have it as a destination. This label is
  // useful for instance for localizations.
  
  class source_question: public element<>
  {
    HX2A_ELEMENT(source_question, type_tag<"question">, element,
		 ((_label, label_tag),
		  (_transitions, transitions_tag)));
  public:

    using transitions_type = own_vector<source_transition>;

    source_question(const string& label):
      _label(*this, label),
      _transitions(*this)
    {
    }

    // For non template questions.
    // The question is found from the localization.
    source_question(const question_localization_r&);

    // It'll add the newly created question to the questionnaire. This is not done by the caller so that referential integrity is happy when the question localization
    // is created.
    // Dummy base class implementation.
    virtual pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&){
      HX2A_ASSERT(false);
      question_r q = make<question>("");
      return {q, make<question_localization>(q, make<question_localization_body>(""))};
    }

    // The question number is used to check if parameter questions are before, on the question or after.
    void compile(const question_infos_by_label_map& qblm, const question_info& qi, questionnaire::questions_type::const_iterator i, questionnaire::questions_type::const_iterator e) const {
      HX2A_ASSERT(*i);
      compile_transitions(qblm, qi, i, e);
      compile_more(qblm, qi, i, e);
    }

    // Passing the question number (from 0), the iterator to the question, the iterator pointing at the end.
    // Doing nothing by default, offering an opportunity for some sub types do something. Obviously some downcasting will be necessary on the question.
    virtual void compile_more(const question_infos_by_label_map&, const question_info&, questionnaire::questions_type::const_iterator, questionnaire::questions_type::const_iterator) const {
      // Nothing by default.
    }
    
    // Iterator in the questionaire pointing at the question being compiled passed instead of the question itself so that the function can look ahead, not just at the question being
    // compiled.
    // Passed by value so that the function can do whatever it wants with the iterator.
    // The question number is used to check if parameter questions are before, on the question or after.
    void compile_transitions(const question_infos_by_label_map&, const question_info&, questionnaire::questions_type::const_iterator i, questionnaire::questions_type::const_iterator e) const;

    slot<string> _label;
    transitions_type _transitions;
  };

  // Specializations for source_question.
  
  // For non-template questions which have text and a style.
  class source_question_inline: public source_question
  {
    HX2A_ELEMENT(source_question_inline, type_tag<"inline">, source_question,
		 ((_style, style_tag),
		  (_text, text_tag)));
  public:

    source_question_inline(
			   const string& label,
			   const string& style,
			   const source_text_r& text
			   ):
      source_question(label),
      _style(*this, style),
      _text(*this, text)
    {
    }

    // Compiling the source text and its functions.
    void compile_more(const question_infos_by_label_map&, const question_info&, questionnaire::questions_type::const_iterator, questionnaire::questions_type::const_iterator) const override;

    virtual void compile_even_more(const question_infos_by_label_map&, const question_info&, questionnaire::questions_type::const_iterator, questionnaire::questions_type::const_iterator) const {
      // Nothing, only for sub types.
    }
    
    slot<string> _style;
    // Every localization in a source question will need the text of the question/message.
    own<source_text> _text;
  };

  class source_question_message: public source_question_inline
  {
    HX2A_ELEMENT(source_question_message, type_tag<"message">, source_question_inline,
		 ());
  public:

    using source_question_inline::source_question_inline;
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };

  // The comment is optional.
  class source_question_with_comment: public source_question_inline
  {
    HX2A_ELEMENT(source_question_with_comment, type_tag<"with_comment">, source_question_inline,
		 ((_comment_label, comment_label_tag)));
  public:

    source_question_with_comment(
				 const string& label,
				 const string& style,
				 const source_text_r& text,
				 const string& comment_label
				 ):
      source_question_inline(label, style, text),
      _comment_label(*this, comment_label)
    {
    }
    
    slot<string> _comment_label;
  };

  // A question with an input field, and an optional comment with another input field.
  class source_question_input: public source_question_with_comment
  {
    HX2A_ELEMENT(source_question_input, type_tag<"input">, source_question_with_comment,
		 ((_optional, optional_tag)));
  public:

    source_question_input(serial_t):
      source_question_with_comment(serial),
      _optional(*this, false) // Not optional by default.
    {
    }

    source_question_input(
			  const string& label,
			  const string& style,
			  const source_text_r& text,
			  const string& comment_label,
			  bool optional
			  ):
      source_question_with_comment(label, style, text, comment_label),
      _optional(*this, optional)
    {
    }
    
    // The second element of the pair is a regular ptr, and not an rfr, because not all source questions come with a
    // localization. For instance a question from template does not.
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;

    slot<bool> _optional;
  };

  class source_question_with_options: public source_question_with_comment
  {
    HX2A_ELEMENT(source_question_with_options, type_tag<"with_options">, source_question_with_comment,
		 ((_options, options_tag),
		  (_randomize, randomize_tag)));
  public:

    source_question_with_options(serial_t):
      source_question_with_comment(serial),
      _options(*this),
      _randomize(*this, false) // Randomize is false by default.
    {
    }

    source_question_with_options(
				 const string& label,
				 const string& style,
				 const source_text_r& text,
				 const string& comment_label,
				 bool randomize
				 ):
      source_question_with_comment(label, style, text, comment_label),
      _options(*this),
      _randomize(*this, randomize)
    {
    }
    
    // None of the virtuals are implemented here, this is an intermediary parent type without instances.
    // Shared helpers for derived classes are here, though.
    void compile_options(const question_body_with_options_r& b, const question_localization_body_with_options_r& ql) const {
      ql->options_clear();
      
      for (const auto& sch: _options){
	if (!sch){
	  throw source_question_contains_null_option(_label);
	}
      
	option_r ch = make<option>(sch->_comment_label.get().size());
	b->push_option_back(ch);
	ql->push_option_back(make<option_localization>(_label, ch, sch->_label, sch->_comment_label));
      }
    }

    own_vector<source_option> _options;
    // Rather than multiplying the types or making a template type, we have a boolean driving whether options should be
    // displayed randomized or not by the GUI.
    slot<bool> _randomize;
  };

  class source_question_select: public source_question_with_options
  {
    HX2A_ELEMENT(source_question_select, type_tag<"select">, source_question_with_options,
		 ());
  public:

    using source_question_with_options::source_question_with_options;

    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;

    // No specific data.
  };

  // Parent class for all source questions with options that must be selected up to a limit.
  class source_question_multiple_choices: public source_question_with_options
  {
    HX2A_ELEMENT(source_question_multiple_choices, type_tag<"multiple_choices">, source_question_with_options,
		 ((_limit, limit_tag)));
  public:

    source_question_multiple_choices(
				     const string& l,
				     const string& s,
				     const source_text_r& t,
				     const string& cl,
				     bool randomize,
				     size_t lim
				     ):
      source_question_with_options(l, s, t, cl , randomize),
      _limit(*this, lim)
    {
    }

    // None of the virtuals are implemented here, this is an intermediary parent type without instances.
    // Shared helpers for derived classes are here, though.
    
    template <typename QuestionBodyWithOptions, typename QuestionLocalizationBodyWithOptions>
    pair<question_r, question_localization_p> tmpl_compile(const questionnaire_r& qq){
      adjust_limit(); // If the limit is 0, it becomes the size of the options.
      
      if (_text.get() == nullptr || _text->_value.get().empty()){
	// There needs to be a text. It is the label of the input field.
	throw source_question_text_is_missing(_label);
      }

      // Number of options equal to limit is fine.
      if (_limit <= 1 || _options.size() < _limit){
	throw source_question_has_invalid_options(_label);
      }
      
      question_body_with_options_r b = make<QuestionBodyWithOptions>(_style, _randomize, _comment_label.get().size(), _limit);
      question_r q = make<question_with_body>(_label, b);
      // We must push the question in the questionnaire so that referential integrity is happy that the localization bears
      // a link to it.
      qq->push_question_back(q);
      rfr<QuestionLocalizationBodyWithOptions> qlb = make<QuestionLocalizationBodyWithOptions>(_text->_value, _comment_label);
      question_localization_r ql = make<question_localization>(q, qlb);
      compile_options(b, qlb);
      return {q, ql};
    }

    // In case the limit is null (or not supplied), it is adjusted to the number of options.
    void adjust_limit(){
      if (!_limit){
	_limit = _options.size();
      }
    }
    
    slot<size_t> _limit;
  };

  class source_question_select_at_most: public source_question_multiple_choices
  {
    HX2A_ELEMENT(source_question_select_at_most, type_tag<"select_at_most">, source_question_multiple_choices,
		 ());
  public:

    using source_question_multiple_choices::source_question_multiple_choices;
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };

  class source_question_select_limit: public source_question_multiple_choices
  {
    HX2A_ELEMENT(source_question_select_limit, type_tag<"select_limit">, source_question_multiple_choices,
		 ());
  public:

    using source_question_multiple_choices::source_question_multiple_choices;
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };

  class source_question_rank_at_most: public source_question_multiple_choices
  {
    HX2A_ELEMENT(source_question_rank_at_most, type_tag<"rank_at_most">, source_question_multiple_choices,
		 ());
  public:

    using source_question_multiple_choices::source_question_multiple_choices;
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };

  class source_question_rank_limit: public source_question_multiple_choices
  {
    HX2A_ELEMENT(source_question_rank_limit, type_tag<"rank_limit">, source_question_multiple_choices,
		 ());
  public:

    using source_question_multiple_choices::source_question_multiple_choices;
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };
  
  class source_question_from_template: public source_question
  {
    HX2A_ELEMENT(source_question_from_template, type_tag<"from_template">, source_question,
		 ((_template_name, template_name_tag)));
  public:

    source_question_from_template(const string& label, const string& template_name):
      source_question(label),
      _template_name(*this, template_name)
    {
    }

    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
    
    slot<string> _template_name; // Name of the template question, if any.
  };

  class source_question_begin_loop: public source_question
  {
    HX2A_ELEMENT(source_question_begin_loop, type_tag<"begin_loop">, source_question,
		 ((_question, question_tag),
		  (_variable, variable_tag),
		  (_operand, operand_tag)));
  public:

    source_question_begin_loop(const string& label, const string& q, const string& variable, const string& operand):
      source_question(label),
      _question(*this, q),
      _variable(*this, variable),
      _operand(*this, operand)
    {
    }

    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;

    // The label of the question whose answer is to iterate upon.
    slot<string> _question; 
    // The name of the loop variable.
    slot<string> _variable; 
    // The code calculating the operand from the answer value.
    // Remove spaces for speed.
    slot<string> _operand; 
  };

  class source_question_end_loop: public source_question
  {
    HX2A_ELEMENT(source_question_end_loop, type_tag<"end_loop">, source_question,
		 ());
  public:

    source_question_end_loop(const string& label):
      source_question(label)
    {
    }
    
    pair<question_r, question_localization_p> compile(const questionnaire_r&, const question_infos_by_label_map&) override;
  };

  // End of specializations for source_question.

  class source_questionnaire: public header_payload
  {
    HX2A_ELEMENT(source_questionnaire, type_tag<"questionnaire">, header_payload,
		 ((_code, code_tag),
		  (_name, name_tag),
		  (_language, language_tag),
		  (_questions, questions_tag)));
  public:

    using questions_type = own_vector<source_question>;
    
    source_questionnaire(serial_t):
      header_payload(serial),
      _code(*this),
      _name(*this),
      _language(*this, language::lang_eng), // English by default.
      _questions(*this)
    {
    }

    source_questionnaire(const questionnaire_r&, const questionnaire_localization_r&);
    
    // Interviews allows two modes of client/server interactions:
    //
    // - Uploading an entire questionnaire which has been edited offline (simply with a JSON editor or a GUI).
    // - Interactively editing a questionnaire, question by question.
    //
    // In both cases, whatever is sent to the server contains references from question to question through their
    // unique labels. These references get "compiled" into Metaspex links as present in the ontology.
    // That way Metaspex's referential integrity performs complex tasks that maintain the whole questionnaire.
    // Service payloads are "source" questions.
    //
    // When uploading an entire questionnaire the following checks are made:
    //
    // 1 - All question labels are unique in a questionnaire.
    // 2 - All question labels are acceptable as JavaScript variable names for conditions to be properly expressed.
    //     The labels must be of the form: [a-zA-Z$][0-9a-zA-Z_$]*, which is a subset of JavaScript variable names
    //     allowed.
    // 3 - All questions transition to an existing different subsequent question. Transitioning backwards is forbidden.
    // 4 - No question is an orphan except the first one. This means that every question but the first one is the
    //     target of a transition.
    //
    // Performs all the checks for the upload of the entire source questionnaire (this), and if everything is correct,
    // creates the questionnaire and the first localization, and returns both.
    pair<questionnaire_r, questionnaire_localization_r> compile(const db::connector& c) const;

    slot<string> _code;
    slot<string> _name;
    slot<language_t> _language;
    // The list is ordered. All questions should have transitions to subsequent questions, no previous one.
    own_vector<source_question> _questions;
  };

  // Localization payloads.
  
  // Source questionnaire.
  // It contains also a first localization. The corresponding language is on the source questionnaire.
  
  // This type is different from source_question. A source question contains potentially
  // additional data, on top of localized labels. As an example, source questions with options
  // contain a randomize boolean that gives instructions to the GUI to shuffle the options.
  // So the types in the hierarchy below contain only the subset of source questions that
  // pertains to localized labels.
  class source_question_localization: public element<>
  {
    HX2A_ELEMENT(source_question_localization, type_tag<"l10n">, element,
		 ((_label, label_tag),
		  (_text, text_tag)));
  public:

    source_question_localization(const question_localization_r& ql):
      _label(*this, ql->get_question()->get_label()),
      _text(*this, ql->get_body()->get_text())
    {
    }
    
    question_localization_r compile(const question_infos_by_label_map&);

    // Dummy implementation.
    // Non const so that specializations can do whatever they want.
    virtual question_localization_r compile(const question_r& q){
      HX2A_ASSERT(false);
      return make<question_localization>(q, make<question_localization_body>(""));
    }
    
    // To identify which question the localization pertains to.
    slot<string> _label;
    slot<string> _text;
  };

  // Specializations of source_question_localization.

  class source_question_localization_message: public source_question_localization
  {
    HX2A_ELEMENT(source_question_localization_message, type_tag<"l10n_message">, source_question_localization,
		 ());
  public:

    using source_question_localization::source_question_localization;
    
    question_localization_r compile(const question_r& q) override;
  };

  // The comment is optional.
  class source_question_localization_with_comment: public source_question_localization
  {
    HX2A_ELEMENT(source_question_localization_with_comment, type_tag<"l10n_with_comment">, source_question_localization,
		 ((_comment_label, comment_label_tag)));
  public:

    source_question_localization_with_comment(const question_localization_r& ql):
      source_question_localization(ql),
      _comment_label(*this)
    {
      auto qlwo = checked_cast<question_localization_body_with_comment>(ql->get_body());
      _comment_label= qlwo->get_comment_label();
    }
    
    slot<string> _comment_label;
  };
					     
  class source_question_localization_input: public source_question_localization_with_comment
  {
    HX2A_ELEMENT(source_question_localization_input, type_tag<"l10n_input">, source_question_localization_with_comment,
		 ());
  public:

    using source_question_localization_with_comment::source_question_localization_with_comment;
    
    question_localization_r compile(const question_r& q) override;
  };
					     
  class source_question_localization_with_options: public source_question_localization_with_comment
  {
    HX2A_ELEMENT(source_question_localization_with_options, type_tag<"l10n_with_options">, source_question_localization_with_comment,
		 ((_options, options_tag)));
  public:

    source_question_localization_with_options(const question_localization_r& ql):
      source_question_localization_with_comment(ql),
      _options(*this)
    {
      auto qlbwo = checked_cast<question_localization_body_with_options>(ql->get_body());
      // Must take care of the options.
      auto i = qlbwo->options_cbegin();
      auto e = qlbwo->options_cend();
      
      while (i != e){
	option_localization_p if_ol = *i;
	HX2A_ASSERT(if_ol);
	option_localization_r ol = *if_ol;
	
	_options.push_back(make<source_option_localization>(ol->get_label(), ol->get_comment_label()));
	++i;
      }
    }

    template <typename QuestionLocalizationBodyType>
    question_localization_r tmpl_compile(const question_r& q) const {
      auto qbwo = checked_cast<question_body_with_options>(q->get_body());
      
      if (_options.size() != qbwo->get_options_size()){
	throw question_localization_options_size_is_incorrect(_label);
      }
      
      auto qlb = make<QuestionLocalizationBodyType>(_text, _comment_label);
      auto i = qbwo->options_cbegin();
    
      // Taking care of options.
      for (const auto& sch: _options){
	if (!sch){
	  throw source_question_contains_null_option(_label);
	}
      
	qlb->push_option_back(make<option_localization>(q->get_label(), **i, sch->_label, sch->_comment_label));
	++i;
      }
      
      return make<question_localization>(q, qlb);
    }
    
    own_vector<source_option_localization> _options;
  };
					     
  class source_question_localization_select: public source_question_localization_with_options
  {
    HX2A_ELEMENT(source_question_localization_select, type_tag<"l10n_select">, source_question_localization_with_options,
		 ());
  public:

    using source_question_localization_with_options::source_question_localization_with_options;
    
    question_localization_r compile(const question_r& q) override;
  };

  class source_question_localization_select_at_most: public source_question_localization_with_options
  {
    HX2A_ELEMENT(source_question_localization_select_at_most, type_tag<"l10n_select_at_most">, source_question_localization_with_options,
		 ());
  public:

    using source_question_localization_with_options::source_question_localization_with_options;

    source_question_localization_select_at_most(const question_localization_r&);
    
    question_localization_r compile(const question_r& q) override;
  };
					     
  class source_question_localization_select_limit: public source_question_localization_with_options
  {
    HX2A_ELEMENT(source_question_localization_select_limit, type_tag<"l10n_select_limit">, source_question_localization_with_options,
		 ());
  public:

    using source_question_localization_with_options::source_question_localization_with_options;
    
    question_localization_r compile(const question_r& q) override;
  };
					     
  class source_question_localization_rank_at_most: public source_question_localization_with_options
  {
    HX2A_ELEMENT(source_question_localization_rank_at_most, type_tag<"l10n_rank_at_most">, source_question_localization_with_options,
		 ());
  public:

    using source_question_localization_with_options::source_question_localization_with_options;
    
    question_localization_r compile(const question_r& q) override;
  };
					     
  class source_question_localization_rank_limit: public source_question_localization_with_options
  {
    HX2A_ELEMENT(source_question_localization_rank_limit, type_tag<"l10n_rank_limit">, source_question_localization_with_options,
		 ());
  public:

    using source_question_localization_with_options::source_question_localization_with_options;
    
    question_localization_r compile(const question_r& q) override;
  };
					     
  // No localization for a question from template.

  // End of specializations of source_question_localization.
  
  class source_questionnaire_localization: public element<>
  {
    HX2A_ELEMENT(source_questionnaire_localization, type_tag<"questionnaire_l10n">, element,
		 ((_questionnaire_id, questionnaire_id_tag),
		  (_title, title_tag),
		  (_language, language_tag),
		  (_questions, questions_tag)));
  public:

    using questions_type = own_vector<source_question>;
    
    source_questionnaire_localization(serial_t):
      element(serial),
      _questionnaire_id(*this),
      _title(*this), // The title of the questionnaire is localized. It is displayed on each localized question if non empty.
      _language(*this, language::lang_eng), // English by default.
      _questions(*this)
    {
    }

    source_questionnaire_localization(const questionnaire_localization_r&);
    
    // Validates the localization and if successful returns a newly created localization.
    // As a design choice, it does not look at the template library. This is done dynamically to allow
    // flexibility (at the expense of an additional cost when calculating the next question during
    // an interview).
    questionnaire_localization_r compile() const;
    
    slot<doc_id> _questionnaire_id;
    slot<string> _title;
    slot<language_t> _language;
    // The list is not ordered.
    own_vector<source_question_localization> _questions;
  };

  // Questionnaire localization payloads.
  
  // Differs from query_id only on the tag.
  class questionnaire_localization_id: public element<>
  {
    HX2A_ELEMENT(questionnaire_localization_id, type_tag<"questionnaire_localization_id">, element,
		 ((_questionnaire_localization_id, questionnaire_localization_id_tag)));
  public:

    slot<doc_id> _questionnaire_localization_id;
  };

  // Interview payloads.

  // Miscellaneous.

  class questionnaire_and_localization_ids: public element<>
  {
    HX2A_ELEMENT(questionnaire_and_localization_ids, type_tag<"questionnaire_and_localization_ids">, element,
		 ((_questionnaire_id, questionnaire_id_tag),
		  (_localization_id, questionnaire_localization_id_tag)));
  public:

    // Strong types for code verification.
    questionnaire_and_localization_ids(const questionnaire_r& q, const questionnaire_localization_r& ql):
      _questionnaire_id(*this, q->get_id()),
      _localization_id(*this, ql->get_id())
    {
    }

    slot<doc_id> _questionnaire_id;
    slot<doc_id> _localization_id;
  };

  // Differs from query_id only on the tag.
  class interview_id_payload: public element<>
  {
    HX2A_ELEMENT(interview_id_payload, type_tag<"interview_id_pld">, element,
		 ((_interview_id, interview_id_tag)));
  public:

    slot<doc_id> _interview_id;
  };

  // Same with a language.
  class interview_id_and_language_payload: public interview_id_payload
  {
    HX2A_ELEMENT(interview_id_and_language_payload, type_tag<"interview_id_and_lang_pld">, interview_id_payload,
		 ((_language, language_tag)));
  public:

    interview_id_and_language_payload(serial_t):
      interview_id_payload(serial),
      _language(*this, language::lang_eng) // Defaults to english.
    {
    }

    slot<language_t> _language;
  };

  class interview_id_and_index_payload: public interview_id_payload
  {
    HX2A_ELEMENT(interview_id_and_index_payload, type_tag<"interview_id_and_index_pld">, interview_id_payload,
		 ((_index, index_tag)));
  public:

    interview_id_and_index_payload(serial_t):
      interview_id_payload(serial),
      _index(*this, 0)
    {
    }

    slot<size_t> _index;
  };

  class interview_start_payload: public element<>
  {
    HX2A_ELEMENT(interview_start_payload, type_tag<"interview_start_pld">, element,
		 ((_interview_id, interview_id_tag),
		  (_interviewee_id, interviewee_id_tag),
		  (_interviewer_id, interviewer_id_tag),
		  (_language, language_tag),
		  (_geo_location, geolocation_tag)));
  public:

    interview_start_payload(serial_t):
      element(serial),
      _interview_id(*this),
      _interviewee_id(*this),
      _interviewer_id(*this),
      _language(*this, language::lang_eng), // English by default.
      _geo_location(*this) // geolocation is a type.
    {
    }

    slot<doc_id> _interview_id;
    slot<string> _interviewee_id;
    slot<string> _interviewer_id;
    slot<language_t> _language;
    own<geolocation> _geo_location;
  };

  class choice_payload: public element<>
  {
    HX2A_ELEMENT(choice_payload, type_tag<"choice_pld">, element,
		 ((_index, index_tag),
		  (_comment, comment_tag)));
  public:

    choice_payload(const choice_r& c):
      _index(*this, c->get_index()),
      _comment(*this, c->get_comment())
    {
    }

    // Starts at 0.
    slot<size_t> _index;
    slot<string> _comment;
  };

  // An answer contents. This type is highly polymorphic, as it contains various kinds of GUI-captured data,
  // depending on the question type the answer corresponds to. 
  class answer_payload: public element<>
  {
    HX2A_ELEMENT(answer_payload, type_tag<"answer_pld">, element,
		 ((_interview_id, interview_id_tag),
		  (_geo_location, geolocation_tag)));
  public:

    answer_payload(
		   const doc_id& interview_id,
		   const geolocation_p& geo_location
		   ):
      _interview_id(*this, interview_id),
      _geo_location(*this, geo_location)
    {
    }

    // Turning the type into an answer as recorded in the interview.
    answer_r make_answer(const question_localization_r& ql, string_view ip_address, time_t elapsed, time_t total_elapsed) const;

    // Same for a question from template.
    answer_r make_answer(const template_question_localization_r& tql, const question_from_template_r& qft, string_view ip_address, time_t elapsed, time_t total_elapsed) const;

    // Dummy base class definition.
    virtual answer_body_r compile(const question_body_r&, const question_localization_body_r&) const {
      HX2A_ASSERT(false);
      return make<answer_body>();
    }
    
    slot<doc_id> _interview_id;
    own<geolocation> _geo_location;
  };

  // Specializations for answer_payload.

  // Contrary to intuition, there is an answer to message, although there is no input to submit. An interstitial message can
  // be added to an interview, just to show a message. The only way to progress is to submit an empty answer. That empty
  // answer is below.
  class answer_message_payload: public answer_payload
  {
    HX2A_ELEMENT(answer_message_payload, type_tag<"answer_message_pld">, answer_payload,
		 ());
  public:
    
    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
  };

  class answer_with_comment_payload: public answer_payload
  {
    HX2A_ELEMENT(answer_with_comment_payload, type_tag<"answer_with_comment_pld">, answer_payload,
		 ((_comment, comment_tag)));
  public:
    
    slot<string> _comment;
  };
  
  class answer_input_payload: public answer_with_comment_payload
  {
    HX2A_ELEMENT(answer_input_payload, type_tag<"answer_input_pld">, answer_with_comment_payload,
		 ((_input, input_tag)));
  public:

    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
    
    slot<string> _input;
  };

  class answer_select_payload: public answer_with_comment_payload
  {
    HX2A_ELEMENT(answer_select_payload, type_tag<"answer_select_pld">, answer_with_comment_payload,
		 ((_choice, choice_tag)));
  public:

    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
    
    own<choice_payload> _choice;
  };

  class answer_multiple_choices_payload: public answer_with_comment_payload
  {
    HX2A_ELEMENT(answer_multiple_choices_payload, type_tag<"answer_multiple_choices_pld">, answer_with_comment_payload,
		 ((_choices, choices_tag)));
  public:

    void push_choice_back(const choice_payload_r& c){
      _choices.push_back(c);
    }

    // Adding the choices to the newly-created answer body.
    // The number of choices has already been checked.
    // We need the question localization, because choices bear a link towards the option localization chosen.
    void shared_compile(const question_localization_body_r& qlb, const answer_body_multiple_choices_r& qbmc) const {
      auto qlbwo = checked_cast<question_localization_body_with_options>(qlb);
	
      for (auto& cp: _choices){
	qbmc->push_choice_back(make<choice>(qlbwo->find_option_localization(cp->_index), cp->_index, cp->_comment));
      }
    }

    own_vector<choice_payload> _choices;
  };

  class answer_select_at_most_payload: public answer_multiple_choices_payload
  {
    HX2A_ELEMENT(answer_select_at_most_payload, type_tag<"answer_select_at_most_pld">, answer_multiple_choices_payload,
		 ());
  public:
    
    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
  };

  class answer_select_limit_payload: public answer_multiple_choices_payload
  {
    HX2A_ELEMENT(answer_select_limit_payload, type_tag<"answer_select_limit_pld">, answer_multiple_choices_payload,
		 ());
  public:

    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
  };

  class answer_rank_at_most_payload: public answer_multiple_choices_payload
  {
    HX2A_ELEMENT(answer_rank_at_most_payload, type_tag<"answer_rank_at_most_pld">, answer_multiple_choices_payload,
		 ());
  public:

    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
  };

  class answer_rank_limit_payload: public answer_multiple_choices_payload
  {
    HX2A_ELEMENT(answer_rank_limit_payload, type_tag<"answer_rank_limit_pld">, answer_multiple_choices_payload,
		 ());
  public:

    answer_body_r compile(const question_body_r&, const question_localization_body_r&) const override;
  };

  // End of specializations for answer_payload.

  class submit_answer_payload: public interview_id_payload
  {
    HX2A_ELEMENT(submit_answer_payload, type_tag<"submit_answer_pld">, interview_id_payload,
		 ((_answer, answer_tag)));
  public:
    
    own<answer_payload> _answer;
  };
  
  // To update an answer, adds the answer's index. Cannot just give the label of the question as with loops several
  // answers point at the same question.
  class answer_revise_payload: public submit_answer_payload
  {
    HX2A_ELEMENT(answer_revise_payload, type_tag<"answer_revise_pld">, submit_answer_payload,
		 ((_index, index_tag)));
  public:
    
    slot<size_t> _index;
  };

  class choice_data: public element<>
  {
    HX2A_ELEMENT(choice_data, type_tag<"choice_data_pld">, element,
		 ((_index, index_tag),
		  (_comment, comment_tag)));
  public:

    choice_data(const choice_r& ch):
      _index(*this, ch->get_index()),
      _comment(*this, ch->get_comment())
    {
    }
    
    // Starts at 0.
    slot<size_t> _index;
    slot<string> _comment;
  };

  // Type used when downloading the answer data in an interview.
  class answer_data: public element<>
  {
    HX2A_ELEMENT(answer_data, type_tag<"answer_data">, element,
		 ((_label, label_tag),
		  (_ip_address, ip_address_tag),
		  (_timestamp, timestamp_tag),
		  (_elapsed, elapsed_tag),
		  (_total_elapsed, total_elapsed_tag),
		  (_geo_location, geolocation_tag)));
  public:

    answer_data(
		const string& label,
		const string& ip_address,
		time_t timestamp,
		time_t elapsed,
		time_t total_elapsed,
		const geolocation_p& geo_location
		):
      _label(*this, label),
      _ip_address(*this, ip_address),
      // Although the timestamp is not stored in the ontology, we reconstruct it here for ease of use in transition conditions,
      // or when doing data analysis.
      _timestamp(*this, timestamp),
      _elapsed(*this, elapsed),
      _total_elapsed(*this, total_elapsed),
      _geo_location(*this, geo_location)
    {
    }
      
    answer_data(const answer_r&, time_t start_timestamp);

    // The label must be present, otherwise there is no way to figure out which question the answer is for.
    slot<string> _label;
    slot<string> _ip_address;
    slot<time_t> _timestamp;
    slot<time_t> _elapsed;
    slot<time_t> _total_elapsed;
    own<geolocation> _geo_location;
  };

  // Specializations of answer_data.

  class answer_data_message: public answer_data
  {
    HX2A_ELEMENT(answer_data_message, type_tag<"answer_data_message">, answer_data,
		 ());
  public:

    answer_data_message(
			const string& label,
			const string& ip_address,
			time_t timestamp,
			time_t elapsed,
			time_t total_elapsed,
			const geolocation_p& geo_location
			):
      answer_data(label, ip_address, timestamp, elapsed, total_elapsed, geo_location)
    {
    }
  };
  
  class answer_data_with_comment: public answer_data
  {
    HX2A_ELEMENT(answer_data_with_comment, type_tag<"answer_data_with_comment">, answer_data,
		 ((_comment, comment_tag)));
  public:

    answer_data_with_comment(
			     const string& label,
			     const string& ip_address,
			     time_t timestamp,
			     time_t elapsed,
			     time_t total_elapsed,
			     const geolocation_p& geo_location,
			     const string& comment
			):
      answer_data(label, ip_address, timestamp, elapsed, total_elapsed, geo_location),
      _comment(*this, comment)
    {
    }
    
    slot<string> _comment;
  };
  
  class answer_data_input: public answer_data_with_comment
  {
    HX2A_ELEMENT(answer_data_input, type_tag<"answer_data_input">, answer_data_with_comment,
		 ((_input, input_tag)));
  public:

    answer_data_input(
		      const string& label,
		      const string& ip_address,
		      time_t timestamp,
		      time_t elapsed,
		      time_t total_elapsed,
		      const geolocation_p& geo_location,
		      const string& comment,
		      const string& input
		      ):
      answer_data_with_comment(label, ip_address, timestamp, elapsed, total_elapsed, geo_location, comment),
      _input(*this, input)
    {
    }

    slot<string> _input;
  };

  class answer_data_select: public answer_data_with_comment
  {
    HX2A_ELEMENT(answer_data_select, type_tag<"answer_data_select">, answer_data_with_comment,
		 ((_choice, choice_tag)));
  public:

    answer_data_select(
		       const string& label,
		       const string& ip_address,
		       time_t timestamp,
		       time_t elapsed,
		       time_t total_elapsed,
		       const geolocation_p& geo_location,
		       const choice_payload_r& choice,
		       const string& comment
		       ):
      answer_data_with_comment(label, ip_address, timestamp, elapsed, total_elapsed, geo_location, comment),
      _choice(*this, choice)
    {
    }

    choice_payload_r get_choice() const {
      HX2A_ASSERT(_choice);
      return *_choice;
    }

    own<choice_payload> _choice;
  };
  
  class answer_data_multiple_choices: public answer_data_with_comment
  {
    HX2A_ELEMENT(answer_data_multiple_choices, type_tag<"answer_data_multiple_choices">, answer_data_with_comment,
		 ((_choices, choices_tag)));
  public:

    using choices_type = own_list<choice_payload>;

    // The choices are built separately.
    answer_data_multiple_choices(
				 const string& label,
				 const string& ip_address,
				 time_t timestamp,
				 time_t elapsed,
				 time_t total_elapsed,
				 const geolocation_p& geo_location,
				 const string& comment
				 ):
      answer_data_with_comment(label, ip_address, timestamp, elapsed, total_elapsed, geo_location, comment),
      _choices(*this)
    {
    }

    choices_type::const_iterator choices_cbegin() const { return _choices.cbegin(); }

    choices_type::const_iterator choices_cend() const { return _choices.cend(); }

    void push_choice_back(const choice_payload_r& c){ _choices.push_back(c); }

    choices_type _choices;
  };

  class answer_data_select_at_most: public answer_data_multiple_choices
  {
    HX2A_ELEMENT(answer_data_select_at_most, type_tag<"answer_data_select_at_most">, answer_data_multiple_choices,
		 ());
  public:

    using answer_data_multiple_choices::answer_data_multiple_choices;
  };
  
  class answer_data_select_limit: public answer_data_multiple_choices
  {
    HX2A_ELEMENT(answer_data_select_limit, type_tag<"answer_data_select_limit">, answer_data_multiple_choices,
		 ());
  public:

    using answer_data_multiple_choices::answer_data_multiple_choices;
  };
  
  class answer_data_rank_at_most: public answer_data_multiple_choices
  {
    HX2A_ELEMENT(answer_data_rank_at_most, type_tag<"answer_data_rank_at_most">, answer_data_multiple_choices,
		 ());
  public:

    using answer_data_multiple_choices::answer_data_multiple_choices;
  };
  
  class answer_data_rank_limit: public answer_data_multiple_choices
  {
    HX2A_ELEMENT(answer_data_rank_limit, type_tag<"answer_data_rank_limit">, answer_data_multiple_choices,
		 ());
  public:

    using answer_data_multiple_choices::answer_data_multiple_choices;
  };
  
  // End of specializations of answer_data.
  
  class user_data: public element<>
  {
    HX2A_ELEMENT(user_data, type_tag<"user_data_pld">, element,
		 ((_id, id_tag)));
  public:

    // Strong type for code verification.
    user_data(const user_r& u):
      _id(*this, u->get_id())
    {
    }
    
    slot<doc_id> _id;
  };

  // E.g. to download the entire interview data, without any localization.
  class interview_data: public element<>
  {
    HX2A_ELEMENT(interview_data, type_tag<"interview_data_pld">, element,
		 ((_start_ip_address, start_ip_address_tag),
		  (_start_timestamp, start_timestamp_tag),
		  (_start_geolocation, start_geolocation_tag),
		  (_interviewee_id, interviewee_id_tag),
		  (_interviewer_id, interviewer_id_tag),
		  (_interviewer_user, interviewer_user_tag),
		  (_language, language_tag),
		  (_answers, answers_tag),
		  (_state, state_tag)));
  public:

    interview_data(const interview_r&);
    
    slot<string> _start_ip_address;
    slot<time_t> _start_timestamp;
    own<geolocation> _start_geolocation;
    slot<string> _interviewee_id;
    slot<string> _interviewer_id;
    // Own so that when not present nothing is sent to the client.
    own<user_data> _interviewer_user;
    slot<language_t> _language;
    own_list<answer_data> _answers;
    slot<interview::state_t> _state;
  };

  // Same including localized data, e.g. for review after answering all the questionnaire.
  
  // We use inheritance as we're just adding localized texts/labels.
  class localized_choice_data: public choice_data
  {
    HX2A_ELEMENT(localized_choice_data, type_tag<"localized_choice_data_pld">, choice_data,
		 ((_label, label_tag),
		  (_comment_label, comment_label_tag)));
  public:

    localized_choice_data(const choice_r& ch):
      choice_data(ch),
      _label(*this, ch->get_option_localization()->get_label()),
      _comment_label(*this, ch->get_option_localization()->get_comment_label())
    {
    }

    // To localize in a language other than the one chosen for the interview.
    localized_choice_data(const choice_r& ch, const option_localization_r& ol):
      // This stays the same.
      choice_data(ch),
      // These must be localized using the option localization.
      _label(*this, ol->get_label()),
      _comment_label(*this, ol->get_comment_label())
    {
    }

    slot<string> _label;
    slot<string> _comment_label;
  };

  // Type used to view an interview answer with the original or a selected localization. It contains both the question and
  // the answers.
  class localized_answer_data: public element<>
  {
    HX2A_ELEMENT(localized_answer_data, type_tag<"l7d_answer_data">, element,
		 ((_label, label_tag),
		  (_text, text_tag)));
  public:

    localized_answer_data(
			  const string& label,
			  const string& text
			  ):
      _label(*this, label),
      _text(*this, text)
    {
    }
    
    slot<string> _label;
    slot<string> _text;
  };

  // Specializations of localized_answer_data.

  class localized_answer_data_message: public localized_answer_data
  {
    HX2A_ELEMENT(localized_answer_data_message, type_tag<"l7d_answer_data_message">, localized_answer_data,
		 ());
  public:

    localized_answer_data_message(
				  const string& label,
				  const string& text
				):
      localized_answer_data(label, text)
    {
    }
  };
  
  class localized_answer_data_with_comment: public localized_answer_data
  {
    HX2A_ELEMENT(localized_answer_data_with_comment, type_tag<"l7d_answer_data_with_comment">, localized_answer_data,
		 ((_comment_label, comment_label_tag),
		  (_comment, comment_tag)));
  public:

    localized_answer_data_with_comment(
				       const string& label,
				       const string& text,
				       const string& comment_label,
				       const string& comment
				):
      localized_answer_data(label, text),
      _comment_label(*this, comment_label),
      _comment(*this, comment)
    {
    }
    
    slot<string> _comment_label;
    slot<string> _comment;
  };

  class localized_answer_data_input: public localized_answer_data_with_comment
  {
    HX2A_ELEMENT(localized_answer_data_input, type_tag<"l7d_answer_data_input">, localized_answer_data_with_comment,
		 ((_input, input_tag)));
  public:

    localized_answer_data_input(
				const string& label,
				const string& text,
				const string& comment_label,
				const string& comment,
				const string& input
				):
      localized_answer_data_with_comment(label, text, comment_label, comment),
      _input(*this, input)
    {
    }
    
    slot<string> _input;
  };
  
  class localized_answer_data_with_options: public localized_answer_data_with_comment
  {
    HX2A_ELEMENT(localized_answer_data_with_options, type_tag<"l7d_answer_data_with_options">, localized_answer_data_with_comment,
		 ((_options, options_tag)));
  public:

    localized_answer_data_with_options(
				       const string& label,
				       const string& text,
				       const string& comment_label,
				       const string& comment
				       ):
      localized_answer_data_with_comment(label, text, comment_label, comment),
      _options(*this)
    {
    }
    
    own_vector<source_option> _options;
  };
  
  class localized_answer_data_select: public localized_answer_data_with_options
  {
    HX2A_ELEMENT(localized_answer_data_select, type_tag<"l7d_answer_data_select">, localized_answer_data_with_options,
		 ((_choice, choice_tag)));
  public:

    localized_answer_data_select(
				 const string& label,
				 const string& text,
				 const string& comment_label,
				 const string& comment
				 ):
      localized_answer_data_with_options(label, text, comment_label, comment),
      _choice(*this)
    {
    }
    
    // The localization is already on the options, we just need the index.
    own<choice_payload> _choice;
  };
  
  class localized_answer_data_multiple_choices: public localized_answer_data_with_options
  {
    HX2A_ELEMENT(localized_answer_data_multiple_choices, type_tag<"l7d_answer_data_multiple_choices">, localized_answer_data_with_options,
		 ((_choices, choices_tag)));
  public:

    localized_answer_data_multiple_choices(
					   const string& label,
					   const string& text,
					   const string& comment_label,
					   const string& comment
					   ):
      localized_answer_data_with_options(label, text, comment_label, comment),
      _choices(*this)
    {
    }

    // The localization is already on the options, we just need the index.
    own_vector<choice_payload> _choices;
  };
  
  class localized_answer_data_select_at_most: public localized_answer_data_multiple_choices
  {
    HX2A_ELEMENT(localized_answer_data_select_at_most, type_tag<"l7d_answer_data_select_at_most">, localized_answer_data_multiple_choices,
		 ());
  public:

    using localized_answer_data_multiple_choices::localized_answer_data_multiple_choices;
  };
  
  class localized_answer_data_select_limit: public localized_answer_data_multiple_choices
  {
    HX2A_ELEMENT(localized_answer_data_select_limit, type_tag<"l7d_answer_data_select_limit">, localized_answer_data_multiple_choices,
		 ());
  public:

    using localized_answer_data_multiple_choices::localized_answer_data_multiple_choices;
  };
  
  class localized_answer_data_rank_at_most: public localized_answer_data_multiple_choices
  {
    HX2A_ELEMENT(localized_answer_data_rank_at_most, type_tag<"l7d_answer_data_rank_at_most">, localized_answer_data_multiple_choices,
		 ());
  public:

    using localized_answer_data_multiple_choices::localized_answer_data_multiple_choices;
  };
  
  class localized_answer_data_rank_limit: public localized_answer_data_multiple_choices
  {
    HX2A_ELEMENT(localized_answer_data_rank_limit, type_tag<"l7d_answer_data_rank_limi">, localized_answer_data_multiple_choices,
		 ());
  public:

    using localized_answer_data_multiple_choices::localized_answer_data_multiple_choices;
  };
  
  // End of specializations of localized_answer_data.

  // Same but with a boolean indicator saying whether there is more in the direction where the answer was fetched.
  class localized_answer_data_and_more_payload: public element<>
  {
    HX2A_ELEMENT(localized_answer_data_and_more_payload, type_tag<"l7d_answer_data_and_more_pld">, element,
		 ((_answer, answer_tag),
		  (_index, index_tag),
		  (_more, more_tag)));
  public:

    localized_answer_data_and_more_payload(
					   const localized_answer_data_r& a,
					   size_t index,
					   bool more
					   ):
      _answer(*this, a),
      _index(*this, index),
      _more(*this, more)
    {
    }

    own<localized_answer_data> _answer;
    slot<size_t> _index;
    slot<bool> _more;
  };
  
  // Type used to view a whole interview with the original or a selected localization.
  class localized_interview_data: public element<>
  {
    HX2A_ELEMENT(localized_interview_data, type_tag<"l7d_interview_data">, element,
		 ((_interviewee_id, interviewee_id_tag),
		  (_interviewer_id, interviewer_id_tag),
		  (_interviewer_user, interviewer_user_tag),
		  (_language, language_tag),
		  (_answers, answers_tag),
		  (_state, state_tag)));
  public:

    localized_interview_data(const interview_r&);
    
    // To localize in a language other than the one chosen for the interview.
    // The ctor checks:
    // - Whether a localization exists with that language.
    // - Whether that localization checks fine.
    // - If the language is not identical to the one the interview was made in.
    //   In that case, the cheaper interview data are calculated.
    localized_interview_data(const interview_r&, language_t);
    
    slot<string> _interviewee_id;
    slot<string> _interviewer_id;
    // Own so that when not present nothing is sent to the client.
    own<user_data> _interviewer_user;
    slot<language_t> _language;
    own_list<localized_answer_data> _answers;
    slot<interview::state_t> _state;
  };
  
  // Campaign payloads.
  
  class campaign_data: public element<>
  {
    HX2A_ELEMENT(campaign_data, type_tag<"campaign_pld">, element,
		 ((_name, name_tag),
		  (_questionnaire_id, questionnaire_id_tag),
		  (_start, start_tag),
		  (_duration, duration_tag),
		  (_interview_lifespan, interview_lifespan_tag)));
  public:

    campaign_data(serial_t):
      element(serial),
      _name(*this),
      _questionnaire_id(*this),
      _start(*this, 0),
      _duration(*this, 0),
      _interview_lifespan(*this, 0) // Infinite.
    {
    }

    campaign_data(const campaign_r& c):
      _name(*this, c->get_name()),
      _questionnaire_id(*this, c->get_questionnaire()->get_id()),
      _start(*this, c->get_start()),
      _duration(*this, c->get_duration()),
      _interview_lifespan(*this, c->get_interview_lifespan())
    {
    }

    slot<string> _name;
    slot<doc_id> _questionnaire_id;
    slot<time_t> _start;
    slot<time_t> _duration;
    slot<time_t> _interview_lifespan;
  };

  using campaign_create_payload = campaign_data;
  using campaign_get_payload = campaign_data;
  using campaign_get_payload_p = ptr<campaign_get_payload>;
  
  // Adding the document identifier of the campaign.
  class campaign_update_payload: public campaign_data
  {
    HX2A_ELEMENT(campaign_update_payload, type_tag<"campaign_update_pld">, campaign_data,
		 ((_campaign_id, campaign_id_tag)));
  public:

    slot<doc_id> _campaign_id;
  };
  
  // Differs from query_id only on the tag.
  class campaign_id: public element<>
  {
    HX2A_ELEMENT(campaign_id, type_tag<"campaign_id">, element,
		 ((_campaign_id, campaign_id_tag)));
  public:

    slot<doc_id> _campaign_id;
  };

  // This type is used to give to the client the next question in the language selected. This is why
  // it is "localized".
  // So, like the question type, it is polymorphic. 

  class localized_question: public header_payload
  {
    HX2A_ELEMENT(localized_question, type_tag<"l7d_question">, header_payload,
		 ((_label, label_tag),
		  (_style, style_tag),
		  (_text, text_tag),
		  (_progress, progress_tag)));
  public:

    localized_question(
		       const string& label,
		       const string& logo,
		       const string& title,
		       const string& style,
		       const string& text,
		       progress_t progress
		       ):
      header_payload(logo, title),
      _label(*this, label),
      _style(*this, style),
      _text(*this, text),
      _progress(*this, progress)
    {
    }

    // Having the label allows the GUI to view the previous question.
    slot<string> _label;
    slot<string> _style;
    slot<string> _text;
    // This is just an indicator. In case of loop in the questionnaire, it won't take them into account.
    // No need to have more than an unsigned integral type giving the %.
    slot<progress_t> _progress;
  };

  // Specializations for localized_question.

  class localized_question_message: public localized_question
  {
    HX2A_ELEMENT(localized_question_message, type_tag<"l7d_question_message">, localized_question,
		 ((_is_final, is_final_tag)));
  public:
    
    // A message is not necessarily final.
    localized_question_message(
			       const string& label,
			       const string& logo,
			       const string& title,
			       const string& style,
			       bool is_final,
			       const string& text,
			       progress_t progress
			       ):
      localized_question(label, logo, title, style, text, progress),
      _is_final(*this, is_final)
    {
    }

    // To indicate to the GUI renderer that it is the last question.
    slot<bool> _is_final;
  };

  class localized_question_with_comment: public localized_question
  {
    HX2A_ELEMENT(localized_question_with_comment, type_tag<"l7d_question_with_comment">, localized_question,
		 ((_comment_label, comment_label_tag)));
  public:

    localized_question_with_comment(
				    const string& label,
				    const string& logo,
				    const string& title,
				    const string& style,
				    const string& text,
				    progress_t progress,
				    const string& comment_label
				    ):
      // Cannot be final.
      localized_question(label, logo, title, style, text, progress),
      _comment_label(*this, comment_label)
    {
    }

    slot<string> _comment_label;
  };

  class localized_question_input: public localized_question_with_comment
  {
    HX2A_ELEMENT(localized_question_input, type_tag<"l7d_question_input">, localized_question_with_comment,
		 ((_optional, optional_tag)));
  public:

    localized_question_input(
			     const string& label,
			     const string& logo,
			     const string& title,
			     const string& style,
			     const string& text,
			     progress_t progress,
			     const string& comment_label,
			     bool optional
			     ):
      localized_question_with_comment(label, logo, title, style, text, progress, comment_label),
      _optional(*this,  optional)
    {
    }

    slot<bool> _optional;
  };
  
  class localized_question_with_options: public localized_question_with_comment
  {
    HX2A_ELEMENT(localized_question_with_options, type_tag<"l7d_question_with_options">, localized_question_with_comment,
		 ((_options, options_tag)));
  public:

    localized_question_with_options(
				    const string& label,
				    const string& logo,
				    const string& title,
				    const string& style,
				    const string& text,
				    progress_t progress,
				    const string& comment_label
				    ):
      localized_question_with_comment(label, logo, title, style, text, progress, comment_label),
      _options(*this)
    {
    }
    
    own_vector<source_option> _options;
  };

  class localized_question_select: public localized_question_with_options
  {
    HX2A_ELEMENT(localized_question_select, type_tag<"l7d_question_select">, localized_question_with_options,
		 ());
  public:

    localized_question_select(
			      const string& label,
			      const string& logo,
			      const string& title,
			      const string& style,
			      const string& text,
			      progress_t progress,
			      const string& comment_label
			      ):
      localized_question_with_options(label, logo, title, style, text, progress, comment_label)
    {
    }
  };
  
  class localized_question_multiple_choices: public localized_question_with_options
  {
    HX2A_ELEMENT(localized_question_multiple_choices, type_tag<"l7d_question_multiple_choices">, localized_question_with_options,
		 ((_limit, limit_tag)));
  public:

    localized_question_multiple_choices(
					const string& label,
					const string& logo,
					const string& title,
					const string& style,
					const string& text,
					progress_t progress,
					const string& comment_label,
					limit_t limit
					):
      localized_question_with_options(label, logo, title, style, text, progress, comment_label),
      _limit(*this, limit)
    {
    }

    slot<limit_t::value_type> _limit;
  };
  
  class localized_question_select_at_most: public localized_question_multiple_choices
  {
    HX2A_ELEMENT(localized_question_select_at_most, type_tag<"l7d_question_select_at_most">, localized_question_multiple_choices,
		 ());
  public:

    localized_question_select_at_most(
				      const string& label,
				      const string& logo,
				      const string& title,
				      const string& style,
				      const string& text,
				      progress_t progress,
				      const string& comment_label,
				      limit_t limit
			      ):
      localized_question_multiple_choices(label, logo, title, style, text, progress, comment_label, limit)
    {
    }
  };
  
  class localized_question_select_limit: public localized_question_multiple_choices
  {
    HX2A_ELEMENT(localized_question_select_limit, type_tag<"l7d_question_select_limit">, localized_question_multiple_choices,
		 ());
  public:

    localized_question_select_limit(
				      const string& label,
				      const string& logo,
				      const string& title,
				      const string& style,
				      const string& text,
				      progress_t progress,
				      const string& comment_label,
				      limit_t limit
			      ):
      localized_question_multiple_choices(label, logo, title, style, text, progress, comment_label, limit)
    {
    }
  };
  
  class localized_question_rank_at_most: public localized_question_multiple_choices
  {
    HX2A_ELEMENT(localized_question_rank_at_most, type_tag<"l7d_question_rank_at_most">, localized_question_multiple_choices,
		 ());
  public:

    localized_question_rank_at_most(
				    const string& label,
				    const string& logo,
				    const string& title,
				    const string& style,
				    const string& text,
				    progress_t progress,
				    const string& comment_label,
				    limit_t limit
				    ):
      localized_question_multiple_choices(label, logo, title, style, text, progress, comment_label, limit)
    {
    }
  };
  
  class localized_question_rank_limit: public localized_question_multiple_choices
  {
    HX2A_ELEMENT(localized_question_rank_limit, type_tag<"l7d_question_rank_limit">, localized_question_multiple_choices,
		 ());
  public:

    localized_question_rank_limit(
				  const string& label,
				  const string& logo,
				  const string& title,
				  const string& style,
				  const string& text,
				  progress_t progress,
				  const string& comment_label,
				  limit_t limit
				  ):
      localized_question_multiple_choices(label, logo, title, style, text, progress, comment_label, limit)
    {
    }
  };
  
  // End specializations for localized_question.
  
  using interview_prepare_payload = campaign_id;

} // End namespace interviews.

#endif
