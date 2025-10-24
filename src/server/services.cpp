//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

// Interviews allows two modes of client/server interactions:
//
// - Uploading an entire questionnaire which has been edited offline (simply with a JSON editor or a GUI).
// - Interactively editing a questionnaire, question by question. The latter is not yet implemented below,
//   but all strong links are in place in the ontology so that small updates propagate the expected changes.
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
// When building a questionnaire interactively the same rules apply, except that 3 and 4 are applied slightly
// differently.
//
// 3: as subsequent questions are usually not created yet, questions creation is made without its transitions.
//    Transitions are added after once the subsequent questions have been created.
// 4: 3 means that this verification is not made immediately. A specific service checks that once the questionnaire
//    is supposed to be complete with all its questions and its transitions.
//
// When uploading an entire questionnaire or creating a new question, a given language has to be supplied, to make
// edition easier. Given that localizations are separate from the questionnaire ontology, a source
// question/questionnaire gets automatically split into a localization chunk/full localization, and the abstraction
// question/questionnaire ontology.
//
// Knowing that the uploaded form is different from the ontology, which is compiled, there are services/functions
// which allow to recreate and download the source form from the compiled/ontology form (pretty much like Excel).
// This requires to give a specific language, and if a localization with that language is found, the source
// can be downloaded.

#include "hx2a/service.hpp"
#include "hx2a/user_session_prologue.hpp"
#include "hx2a/cursor_on_key_range.hpp"
#include "hx2a/cursor_on_key.hpp"
#include "hx2a/paginated_services.hpp"
#include "hx2a/json_leading_value_remover.hpp"
#include "hx2a/projector.hpp"
#include "hx2a/http_request.hpp"
#include "hx2a/db/connector.hpp"
#include "hx2a/build_key.hpp"

#include "hx2a/payloads/language_payload.hpp"
#include "hx2a/payloads/reply_id.hpp"

#include "interviews/ontology.hpp"
#include "interviews/payloads.hpp"

namespace interviews {

  using namespace hx2a;

  // *** Template library services ***

  // Template question category services.

  // Create.

  auto _template_question_category_create = service<srv_tag<"template_question_category_create">>
    ([](const rfr<template_question_category_create_payload>& q){
      db::connector cn{dbname};
      
      template_question_category_p par;
      
      // Let's fetch the parent category, if any.
      if (!q->_parent.get().is_null()){
	par = template_question_category::get(cn, q->_parent);

	if (!par){
	  throw template_question_category_does_not_exist();
	}
      }
      
      return make<reply_id>(make<template_question_category>(*cn, q->_name, par)->get_id());
    });
 
  // Retrieve.

  auto _template_question_category_get = service<srv_tag<"template_question_category_get">>
    ([](const rfr<template_question_category_id>& q){
      db::connector cn{dbname};
      template_question_category_r tqc = template_question_category::get(cn, q->_template_question_category_id).or_throw<template_question_category_does_not_exist>();

      return make<template_question_category_data>(tqc);
    });

  // The situation where there is no parent materializes as an empty JSON object returned.
  auto _template_question_category_get_parent = service<srv_tag<"template_question_category_get_parent">>
    ([](const rfr<template_question_category_id>& q) -> template_question_category_data_p {
      db::connector cn{dbname};
      template_question_category_r tqc = template_question_category::get(cn, q->_template_question_category_id).or_throw<template_question_category_does_not_exist>();

      if (template_question_category_p par = tqc->get_parent()){
	return make<template_question_category_data>(*par);
      }

      return {};
    });
 
  // Paginate.

  // To look only with a single template question category at the beginning of the index.
  struct template_question_category_injector
  {
    row_key_t operator()(const row_key_t& t, const template_question_category_id_r& query){
      return build_key(query->_template_question_category_id, t);
    }
  };
  
  struct template_question_category_remover
  {
    row_key_t operator()(const row_key_t& v){
      return json::remove_leading_values<1>(v);
    }
  };
  
  // Template question categories per parent category.
  paginated_services<
    srv_tag<"template_question_categories_by_parent">,
    template_question_category,
    projector<template_question_category_data>,
    nil_prologue,
    template_question_category_id,
    template_question_category_injector,
    template_question_category_remover
    >
  _template_question_categories_by_parent(config::get_id(dbname), config_name<"tqc_p">);

  // Update.

  auto _template_question_category_update = service<srv_tag<"template_question_category_update">>
    ([](const rfr<template_question_category_update_payload>& q){
      db::connector cn{dbname};
      template_question_category_r tqc = template_question_category::get(cn, q->_template_question_category_id).or_throw<template_question_category_does_not_exist>();

      tqc->update(q->_name);
    });
 
  // Delete.

  auto _template_question_category_remove = service<srv_tag<"template_question_category_remove">>
    ([](const rfr<template_question_category_id>& q){
      db::connector cn{dbname};
      template_question_category_r tqc = template_question_category::get(cn, q->_template_question_category_id).or_throw<template_question_category_does_not_exist>();

      tqc->unpublish();
    });
 
  // Template question services.

  // Create.

  auto _template_question_create = service<srv_tag<"template_question_create">>
    ([](const rfr<template_question_create_payload>& q){
      db::connector cn{dbname};
      template_question_category_r tqc = template_question_category::get(cn, q->_template_question_category_id).or_throw<template_question_category_does_not_exist>();

      source_template_question_r tsq = q->_source_question.or_throw<template_question_misses_question>();
      return make<reply_id>(tsq->compile(tqc)->get_id());
    });
 
  // Retrieve. Beware, the service does not take a template question identifier, but a template question localization id!

  // From a template question localization id.
  auto _template_question_get = service<srv_tag<"template_question_get">>
    ([](const rfr<template_question_get_payload>& q){
      db::connector cn{dbname};
      template_question_localization_r tql = template_question_localization::get(cn, q->_template_question_localization_id).or_throw<template_question_localization_does_not_exist>();

      return make<template_question_data>(tql);
    });

  // From a template question identifier and language. A tad more expensive than the former as it must access an index. Prefer the former one if possible.
  auto _template_question_get_from_lang = service<srv_tag<"template_question_get_from_lang">>
    ([](const rfr<template_question_get_from_language_payload>& q){
      db::connector cn{dbname};
      template_question_r tq = template_question::get(cn, q->_template_question_id).or_throw<template_question_does_not_exist>();
      
      template_question_localization_r tql = template_question_localization::find(tq, q->_language)
	.or_throw<template_question_localization_does_not_exist>();
      return tql->make_source_template_question();
    });
  
  // Paginate.

  struct compute_template_question_data
  {
    template_question_data_r operator()(const template_question_r& source, const template_question_category_id_and_language_payload_r& q) const {
      template_question_localization_r tql = template_question_localization::find(source, q->_language)
	.or_throw<template_question_localization_does_not_exist>();

      return make<template_question_data>(tql);
    }
  };

  // Template question categories per given category.
  paginated_services<
    srv_tag<"template_questions_by_category">,
    template_question,
    compute_template_question_data,
    nil_prologue,
    template_question_category_id_and_language_payload,
    template_question_category_injector, // Here we add the category id.
    template_question_category_remover
    >
  _template_questions_by_category(config::get_id(dbname), config_name<"tq_c">);

  // Update of a template question and template question localization, given a template question localization id.
  //
  // This service updates:
  // - The template question
  //   - The label
  //   - The template question category if one non null is supplied.
  //   - The style.
  //   - All other data on a template question specialization, except the label (see below).
  // - The template question localization
  //   - The language of the localization, in the new language is not yet covered.
  //
  // It cannot update the label of the template question. Remove the template question and create another one with the new label.
  auto _template_question_update = service<srv_tag<"template_question_update">>
    ([](const rfr<template_question_update_payload>& q){
      db::connector cn{dbname};
      template_question_localization_r tql = template_question_localization::get(cn, q->_template_question_localization_id).or_throw<template_question_localization_does_not_exist>();

      template_question_category_p tqc;

      // Giving a template question category id is optional. 
      if (!q->_template_question_category_id.get().is_null()){
	tqc = template_question_category::get(cn, q->_template_question_category_id);
	
	if (!tqc){
	  throw template_question_category_does_not_exist();
	}
      }

      source_template_question_r tsq = q->_source_template_question.or_throw<template_question_misses_question>();
      
      tsq->update(tql);

      if (tqc){
	tql->get_template_question()->set_category(*tqc);
      }
    });
  
  // Delete.

  auto _template_question_remove = service<srv_tag<"template_question_remove">>
    ([](const rfr<template_question_id>& q){
      db::connector cn{dbname};
      // Let's fetch the template_question.
      template_question_r tq = template_question::get(cn, q->_template_question_id).or_throw<template_question_does_not_exist>();
      
      tq->unpublish();
    });

  // Template question localization services.

  // Create.

  // It checks whether a localization with the specified language already exists and throws an exception in case there is.
  auto _template_question_localization_create = service<srv_tag<"template_question_localization_create">>
    ([](const rfr<template_question_localization_create_payload>& q){
      // The compile function connects to the database.
      return make<reply_id>(q->compile()->get_id());
    });

  // Retrieve.

  auto _template_question_localization_get = service<srv_tag<"template_question_localization_get">>
    ([](const rfr<template_question_localization_id>& q){
      db::connector cn{dbname};
      template_question_localization_r tql = template_question_localization::get(cn, q->_template_question_localization_id).or_throw<template_question_localization_does_not_exist>();

      return tql->make_source_template_question_localization();
    });

  // Paginate.

  // List all the template question localizations per template question and language.
  
  struct compute_source_template_question_localization
  {
    source_template_question_localization_r operator()(const template_question_localization_r& source) const {
      return source->make_source_template_question_localization();
    }
  };

  struct template_question_id_adder
  {
    row_key_t operator()(const row_key_t& t, const rfr<template_question_id>& query) const {
      return build_key(query->_template_question_id, t);
    }
  };
  
  // Interview data by campaign and by state.
  paginated_services<
    srv_tag<"template_question_localization_data_by_question">,
    template_question_localization,
    compute_source_template_question_localization,
    nil_prologue,
    template_question_id,
    template_question_id_adder,
    json_leading_value_remover
  >
  _template_question_localization_by_question(config::get_id(dbname), config_name<"tql_q">);

  // Update.

  // Even if all the data are the same, the template question localization update will modify the document, and the document will be saved in database.
  // This is out of convenience to simplify the code.
  auto _template_question_localization_update = service<srv_tag<"template_question_localization_update">>
    ([](const rfr<template_question_localization_update_payload>& q){
      // The update function uses its own database connexion. No need to make one.
      q->update();
    });

  // Delete is not allowed. It would defeat all validations on questionnaire localizations. If a questionnaire localization uses a
  // template question localization, and gets verified, if the template question localization gets removed, the template questionnaire
  // localization becomes invalid. Once created a template question localization can only be updated. It will be automatically removed
  // by referential integrity when the template question it pertains to is itself removed (directly or indirectly through category
  // removal).

  // *** Questionnaire services ***
  
  // Service to upload a whole source questionnaire.
  // It compiles the source questionnaire and potentially reports issues.
  // If the questionnaire has been successfully compiled, it is created, saved and its document identifier is returned.

  auto _questionnaire_upload = service<srv_tag<"questionnaire_upload">>
    ([](const rfr<source_questionnaire>& q){
      db::connector cn{dbname};
      // No need to check if a localization already exists for the questionnaire, we're creating it...
      std::pair<questionnaire_r, questionnaire_localization_r> c = q->compile(*cn);
      HX2A_LOG(trace) << "Questionnaire successfully compiled.";
      return make<questionnaire_and_localization_ids>(c.first, c.second);
    });

  // Service to download a source questionnaire for a given language.

  auto _questionnaire_download = service<srv_tag<"questionnaire_download">>
    ([](const rfr<questionnaire_id_and_language_payload>& q){
      db::connector cn{dbname};
      // Let's fetch the questionnaire.
      questionnaire_r qq = questionnaire::get(cn, q->_questionnaire_id).or_throw<questionnaire_does_not_exist>();
      
      // Now let's fetch the localization, if any.
      questionnaire_localization_r ql = questionnaire_localization::find(qq, q->_language)
	.or_throw<questionnaire_localization_does_not_exist>();
      
      return make<source_questionnaire>(qq, ql);
    });

  // Paginated services to list questionnaires.
  // They are delivered in ENGLISH! The reason for that is that Metaspex allows to add information to the services query, in particular
  // the language the questionnaires must be returned into, but it is not able yet to transmit that query to the projection/computation
  // function. So the English language is built-in.

  struct compute_source_questionnaire
  {
    source_questionnaire_r operator()(const questionnaire_r& source, const rfr<language_payload>& query) const {
      questionnaire_localization_r ql = questionnaire_localization::find(source, query->language.get() ? query->language.get() : language::lang_eng)
	.or_throw<questionnaire_localization_in_english_does_not_exist>();
      
      return make<source_questionnaire>(source, ql);
    }
  };
  
  // Questionnaires by name.
  paginated_services<
    srv_tag<"questionnaires_by_name">,
    questionnaire,
    compute_source_questionnaire,
    nil_prologue,
    language_payload
    >
  _questionnaires_by_name(config::get_id(dbname), config_name<"qq_n">);
  
  // Service to remove a questionnaire.

  auto _questionnaire_remove = service<srv_tag<"questionnaire_remove">>
    ([](const rfr<questionnaire_id>& q){
      db::connector cn{dbname};
      // Let's fetch the questionnaire.
      questionnaire_r qq = questionnaire::get(cn, q->_questionnaire_id).or_throw<questionnaire_does_not_exist>();
      
      qq->unpublish();
    });

  // Interactive edition services below are not yet implemented.
  
  // Service to clone a questionnaire.

  // Services to edit a questionnaire, update a question, remove a question, remove all transitions from a question, add a question before a question, add a question after a question.

  // Service to add transitions to a question.

  // Service to remove the transitions of a question.

  // Service to check if there are no orphans in a questionnaire.

  // *** Questionnaire localizations services ***

  // The service uploads a questionnaire localization. In case the upload is successful and a former localization existed, the latter is removed and replaced.
  // It is not allowed to upload a localization when a campaign has started. The questionnaire becomes locked. Indeed, if interviews are already using an old one,
  // they'll be removed. We could be smarter and check whether a previous localization exists and if it does if interviews already use it, but that would make
  // the code quite complicated and tedious to explain. The rule is therefore simple.
  auto _questionnaire_localize = service<srv_tag<"questionnaire_localize">>
    ([](const rfr<source_questionnaire_localization>& q){
      db::connector cn{dbname};
      // This might throw, as the resulting questionnaire localization is validated (for bijection with questionnaire).
      // The compile function returns a questionnaire localization rfr.
      questionnaire_localization_r ql = q->compile();
      doc_id qlid = ql->get_id();

      // Checking that the language exists.
      if (!language::get_info(q->_language)){
	throw language_code_does_not_exist();
      }
      
      // Now that the questionnaire localization has been successfully compiled and created, we can check if another
      // localization exists, and we must remove it. 
      // Creating a no limit cursor.
      cursor c = cursor_on_key<questionnaire_localization>(cn->get_index(config_name<"qloc_q">),
							   {.key = {ql->get_questionnaire()->get_id(), q->_language},
							    // Should be more configurable than that.
							    .limit = 128});

      while (c.read_next()){
	for (const auto& r: c.get_rows()){
	  // The cursor won't be able to find the one we just created, as we did not commit yet. No need to check.
	  r->unpublish();
	}
      }
      
      return make<reply_id>(qlid);
    });

  // Paginated services to list questionnaire localizations.

  struct compute_source_questionnaire_localization
  {
    source_questionnaire_localization_r operator()(const questionnaire_localization_r& source) const {
      return make<source_questionnaire_localization>(source);
    }
  };
  
  // Questionnaire localizations by questionnaire and language.
  paginated_services<srv_tag<"questionnaire_localizations_by_questionnaire">,
		     questionnaire_localization,
		     compute_source_questionnaire_localization>
  _questionnaire_localizations_by_questionnaire(config::get_id(dbname), config_name<"qloc_q">);
  
  // Service to download a questionnaire localization.

  auto _questionnaire_localization_download = service<srv_tag<"questionnaire_localization_download">>
    ([](const rfr<questionnaire_localization_id>& q){
      db::connector cn{dbname};
      // Retrieving the questionnaire localization.
      questionnaire_localization_r ql = questionnaire_localization::get(cn, q->_questionnaire_localization_id).or_throw<questionnaire_localization_does_not_exist>();
      
      return make<source_questionnaire_localization>(ql);
    });

  // Service to remove a questionnaire localization.

  auto _questionnaire_localization_remove = service<srv_tag<"questionnaire_localization_remove">>
    ([](const rfr<questionnaire_localization_id>& q){
      db::connector cn{dbname};
      // Retrieving the questionnaire localization.
      questionnaire_localization_r ql = questionnaire_localization::get(cn, q->_questionnaire_localization_id).or_throw<questionnaire_localization_does_not_exist>();

      ql->unpublish();
    });

  // *** Campaign services ***

  // Service to create a campaign.

  auto _campaign_create = service<srv_tag<"campaign_create">>
    ([](const rfr<campaign_create_payload>& q){
      db::connector cn{dbname};
      // Let's fetch the questionnaire.
      questionnaire_r qq = questionnaire::get(cn, q->_questionnaire_id).or_throw<questionnaire_does_not_exist>();
      
      return make<reply_id>(make<campaign>(*cn, q->_name, qq, q->_start, q->_duration, q->_interview_lifespan)->get_id());
    });
 
  // Service to retrieve a campaign.

  auto _campaign_get = service<srv_tag<"campaign_get">>
    ([](const rfr<campaign_id>& q){
      db::connector cn{dbname};
      // Let's fetch the campaign.
      campaign_r c = campaign::get(cn, q->_campaign_id).or_throw<campaign_does_not_exist>();

      return make<campaign_get_payload>(c);
    });

  // Services to list campaigns.

  struct compute_campaign_data
  {
    campaign_data_r operator()(const campaign_r& source) const {
      return make<campaign_data>(source);
    }
  };

  // Campaigns by name and start date.
  paginated_services<srv_tag<"campaigns_by_name">, campaign, compute_campaign_data>
  _campaigns_by_name(config::get_id(dbname), config_name<"c_n">);
  
  // Service to update a campaign.

  auto _campaign_update = service<srv_tag<"campaign_update">>
    ([](const rfr<campaign_update_payload>& q){
      db::connector cn{dbname};
      // Let's fetch the campaign.
      campaign_r c = campaign::get(cn, q->_campaign_id).or_throw<campaign_does_not_exist>();
      
      // Let's fetch the questionnaire.
      questionnaire_r qq = questionnaire::get(cn, q->_questionnaire_id).or_throw<questionnaire_does_not_exist>();
      
      c->update(q->_name, qq, q->_start, q->_duration, q->_interview_lifespan);
    });
 
  // Service to remove a campaign.

  auto _campaign_remove = service<srv_tag<"campaign_remove">>
    ([](const rfr<campaign_id>& q){
      db::connector cn{dbname};
      // Let's fetch the campaign.
      campaign_r c = campaign::get(cn, q->_campaign_id).or_throw<campaign_does_not_exist>();

      c->unpublish();
    });
 
  // *** Interview services ***

  // Service to prepare an interview.

  auto _interview_prepare = service<srv_tag<"interview_prepare">>
    ([](const rfr<interview_prepare_payload>& q){
      db::connector cn{dbname};

      // Let's fetch the campaign.
      campaign_r c = campaign::get(cn, q->_campaign_id).or_throw<campaign_does_not_exist>();

      return make<reply_id>(make<interview>(cn, c)->get_id());
    });

  // Service to obtain the languages supported by an interview.

  auto _get_languages = service<srv_tag<"get_languages">>
    ([](const interview_id_payload_r& query){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, query->_interview_id).or_throw<interview_does_not_exist>();

      // Let's fetch the questionnaire.
      questionnaire_r q = i->get_questionnaire();

      // Let's fetch the questionnaire localizations. Unlimited.
      doc_id qid = q->get_id();
      cursor c = cursor_on_key_range<questionnaire_localization>(cn->get_index(config_name<"qloc_q">),
								 {.start = {qid}, .upper_bound = {qid},
								  // Should be more configurable than that.
								  .limit = 128});
      
      // Let's prepare the reply.
      languages_payload_r lp = make<languages_payload>(q->get_logo());

      while (c.read_next()){
	for (const auto& r: c.get_rows()){
	  lp->_languages.push_back(r->get_language());
	}
      }
      
      return lp;
    });

  // Service to start an interview for a given campaign in a given language.
  // It checks if the campaign is active. It also checks the localization.

  auto _interview_start = service<srv_tag<"interview_start">>
    ([](const user_session_prologue& prologue, const rfr<interview_start_payload>& q){
      db::connector cn{dbname};
      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();
      i->check_active();
      // This will fix the next question of the interview to the first question of the questionnaire. There is one.
      i->start(q->_interviewee_id, q->_interviewer_id, prologue.user, q->_language, prologue.request.get_client_ip(), q->_geo_location);
      return i->next_localized_question();
    });
 
  // Service to request the next question for a given interview.
  // It checks if the campaign is still active.
  // It'll return the final question (a message) in case the interview is complete.

  auto _next_question = service<srv_tag<"next_question">>
    ([](const interview_id_payload_r& q){
      db::connector cn{dbname};
      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();
      // We display on and on the last message within the campaign window.
      i->check_live();
      return i->next_localized_question();
    });
 
  // Service to post the answer to the current question of a given interview, it returns the next question
  // if there is one, the empty JSON object in case the interview is complete.
  // It checks if the campaign is still active.
  // The service returns the next localized question to avoid an additional service call to get the next
  // question.
  // The service does not take an "answer_payload" because we need a type to share between this service and
  // the service that allows to revise an answer. That common type is answer_payload.
  auto _answer_srv = service<srv_tag<"answer">>
    ([](const http_request& r, const submit_answer_payload_r& q){
      db::connector cn{dbname};

      if (!q->_answer){
	throw answer_is_missing();
      }
      
      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();
      i->check_live();

      if (i->is_completed()){
	throw interview_is_already_completed();
      }

      localizations locs = i->next_question_localization();
      pair<time_t, time_t> el = i->calculate_elapsed_times();

      std::visit(overloaded(
			    [&](const question_localization_r& l) {
			      // Non template question.
			      i->add_answer(q->_answer->make_answer(l, r.get_client_ip(), el.first, el.second));
			    },
			    [&](const template_localization& l){
			      // Question from template.
			      i->add_answer(q->_answer->make_answer(l.localization, l.question, r.get_client_ip(), el.first, el.second));
			    }),
		 locs);
      
      return i->move_ahead();
    });

  // Service to revise an answer. If the transition is the same as before, the update is accepted without
  // any other change to the interview.
  // If the transition changes, the remainder of the interview is canceled, and the interviewee must continue answering
  // until the interview is complete.
  // If the new transition leads to a question which was already answered, we cannot just erase the intermediary
  // answers, and consider the job complete. We must rerun all the subsequent questions transitions to check that they
  // were not altered, as they might use the revised answer. In case they are unchanged, we can keep the remainder of the
  // interview, complete or not. If one is changed, the remainder of the interview will have to under a similar
  // process of resection.

  auto _answer_revise_srv = service<srv_tag<"answer_revise">>
    ([](const http_request& r, const rfr<answer_revise_payload>& query){
      db::connector cn{dbname};

      if (!query->_answer){
	throw answer_is_missing();
      }
      
      // Let's fetch the interview.
      interview_r i = interview::get(cn, query->_interview_id).or_throw<interview_does_not_exist>();

      // It's not authorized to revise an answer on an interview which is not live.
      i->check_live();

      // We cannot use anchor ids for direct access as we need the iterator to perform revision/resection.
      // One pass done on the interview.
      size_t index = query->_index;
      interview::history_type::iterator pos = i->find_answer(index);
      HX2A_ASSERT(*pos);
      entry_r e = **pos;
      entry_answer* ea = dynamic_cast<entry_answer*>(&e.get());
      HX2A_ASSERT(ea);
      localizations locs = ea->get_answer()->get_question_localization();
      // This does not require a pass on the interview.
      pair<time_t, time_t> el = i->calculate_elapsed_times();

      return std::visit(overloaded(
				   [&](const question_localization_r& l) {
				     // Regular question localization.
				     return i->revise_answer(pos, query->_answer->make_answer(l, r.get_client_ip(), el.first, el.second));
				   },
				   [&](const template_localization& l){
				     // Template question localization.
				     return i->revise_answer(pos, query->_answer->make_answer(l.localization, l.question, r.get_client_ip(), el.first, el.second));
				   }),
			locs);
    });

  // Service to remove an interview.

  auto _interview_remove = service<srv_tag<"interview_remove">>
    ([](const interview_id_payload_r& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();

      i->unpublish();
    });
 
  // Service to download an interview with just the answers, no language. For automated processing.

  auto _interview_get = service<srv_tag<"interview_get">>
    ([](const interview_id_payload_r& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();

      return make<interview_data>(i);
    });
 
  // Service to download an interview in the language it was conducted into. For human consultation.
  // It indicates whether the interview is complete or not.

  auto _interview_original_get = service<srv_tag<"interview_original_get">>
    ([](const interview_id_payload_r& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();

      return make<localized_interview_data>(i);
    });

  auto _interview_previous_answer = service<srv_tag<"prev_answer">>
    ([](const rfr<interview_id_and_index_payload>& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();
      return i->get_previous_answer(q->_index);
    });

  auto _interview_next_answer = service<srv_tag<"next_answer">>
    ([](const rfr<interview_id_and_index_payload>& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();
      return i->get_next_answer(q->_index);
    });
  
  // Service to download an interview in a given language, not necessarily the one it was conducted in.
  // It indicates whether the interview is complete or not.

  auto _interview_localized_get = service<srv_tag<"interview_localized_get">>
    ([](const rfr<interview_id_and_language_payload>& q){
      db::connector cn{dbname};

      // Let's fetch the interview.
      interview_r i = interview::get(cn, q->_interview_id).or_throw<interview_does_not_exist>();

      return make<localized_interview_data>(i, q->_language);
    });

  // Paginated services to list interviews.

  struct campaign_id_adder
  {
    row_key_t operator()(const row_key_t& t, const rfr<campaign_id>& query) const {
      return build_key(query->_campaign_id, t);
    }
  };
  
  // Interview data for a given campaign by state.
  paginated_services<
    srv_tag<"interview_data_by_campaign">,
    interview,
    projector<interview_data>,
    nil_prologue,
    campaign_id,
    campaign_id_adder,
    json_leading_value_remover
  >
  _interviews_by_campaign(config::get_id(dbname), config_name<"i_c">);

} // End namespace interviews.
