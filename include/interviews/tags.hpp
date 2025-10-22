//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#ifndef HX2A_INTERVIEWS_TAGS_HPP
#define HX2A_INTERVIEWS_TAGS_HPP

#include "hx2a/tag_type.hpp"
#include "hx2a/service_name.hpp"

// List of "readable" tags for payloads. Enforces reuse and consistency.

namespace interviews {

  using hx2a::tag_t;

  // Useful for type tags and exceptions.
  // '_' so that JavaScript can use it easily as object keys.
  template <tag_t tag>
  constexpr tag_t type_tag = hx2a::tag_concat<"itv_", tag>;

  // Used for service names.
  template <hx2a::service_name_t tag>
  constexpr hx2a::service_name_t srv_tag = hx2a::srv_concat<"itv_", tag>;

  constexpr tag_t answer_tag                            = "answer";
  constexpr tag_t answers_tag                           = "answers";
  constexpr tag_t body_tag                              = "body";
  constexpr tag_t campaign_id_tag                       = "campaign_id";
  constexpr tag_t choice_tag                            = "choice";
  constexpr tag_t choices_tag                           = "choices";
  constexpr tag_t code_tag                              = "code";
  constexpr tag_t comment_label_tag                     = "comment_label";
  constexpr tag_t comment_tag                           = "comment";
  constexpr tag_t condition_tag                         = "condition";
  constexpr tag_t destination_tag                       = "destination";
  constexpr tag_t duration_tag                          = "duration";
  constexpr tag_t elapsed_tag                           = "elapsed";
  constexpr tag_t functions_tag                         = "functions";
  constexpr tag_t geolocation_tag                       = "geolocation";
  constexpr tag_t id_tag                                = "id";
  constexpr tag_t index_tag                             = "index";
  constexpr tag_t input_tag                             = "input";
  constexpr tag_t interview_id_tag                      = "interview_id";
  constexpr tag_t interview_lifespan_tag                = "interview_lifespan";
  constexpr tag_t interviewee_id_tag                    = "interviewee";
  constexpr tag_t interviewer_id_tag                    = "interviewer";
  constexpr tag_t interviewer_user_tag                  = "interviewer_user";
  constexpr tag_t ip_address_tag                        = "ip_address";
  constexpr tag_t is_final_tag                          = "final";
  constexpr tag_t label_tag                             = "label";
  constexpr tag_t labels_tag                            = "labels";
  constexpr tag_t language_tag                          = "lang";
  constexpr tag_t languages_tag                         = "langs";
  constexpr tag_t limit_tag                             = "limit";
  constexpr tag_t logo_tag                              = "logo";
  constexpr tag_t more_tag                              = "more";
  constexpr tag_t name_tag                              = "name";
  constexpr tag_t operand_tag                           = "operand";
  constexpr tag_t optional_tag                          = "optional";
  constexpr tag_t options_tag                           = "options";
  constexpr tag_t parameters_tag                        = "parameters";
  constexpr tag_t parent_tag                            = "parent";
  constexpr tag_t progress_tag                          = "progress";
  constexpr tag_t question_tag                          = "question";
  constexpr tag_t questionnaire_id_tag                  = "questionnaire_id";
  constexpr tag_t questionnaire_localization_id_tag     = "questionnaire_localization_id";
  constexpr tag_t questions_tag                         = "questions";
  constexpr tag_t randomize_tag                         = "randomize";
  constexpr tag_t start_tag                             = "start";
  constexpr tag_t start_geolocation_tag                 = "start_geolocation";
  constexpr tag_t start_ip_address_tag                  = "start_ip_address";
  constexpr tag_t start_timestamp_tag                   = "start_timestamp";
  constexpr tag_t state_tag                             = "state";
  constexpr tag_t style_tag                             = "style";
  constexpr tag_t template_name_tag                     = "template";
  constexpr tag_t template_question_category_id_tag     = "template_question_category_id";
  constexpr tag_t template_question_id_tag              = "template_question_id";
  constexpr tag_t template_question_localization_id_tag = "template_question_localization_id";
  constexpr tag_t text_tag                              = "text";
  constexpr tag_t timestamp_tag                         = "timestamp";
  constexpr tag_t title_tag                             = "title";
  constexpr tag_t total_elapsed_tag                     = "total_elapsed";
  constexpr tag_t transitions_tag                       = "transitions";
  constexpr tag_t value_tag                             = "value";
  constexpr tag_t variable_tag                          = "variable";

} // End namespace interviews.

#endif
