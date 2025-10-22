//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#ifndef HX2A_INTERVIEWS_EXCEPTION_HPP
#define HX2A_INTERVIEWS_EXCEPTION_HPP

#include "hx2a/exception.hpp"
#include "hx2a/logging.hpp"
#include "hx2a/format.hpp"

#include "interviews/tags.hpp"
#include "interviews/misc.hpp"

namespace interviews {

  using namespace hx2a;

  template <tag_t application_code, tag_t application_message>
  class exception: public application_exception_base
  {
  public:

    exception():
      application_exception_base(string{type_tag<application_code>}, string{application_message})
    {
    }
  };

  // The template exceptions below do not use strong ontology types as argument to avoid any complicated dependencies
  // between ontology and exceptions.
  
  // Exception on a question. Reporting on its label.
  template <tag_t application_code, tag_t application_message>
  class exception_q: public application_exception_base
  {
  public:

    exception_q(const string& question_label):
      application_exception_base(string{type_tag<application_code>}, format(application_message.view(), " At question with label \"", question_label, "\"."))
    {
    }
  };

  // Exception on an answer. Reporting on its index.
  template <tag_t application_code, tag_t application_message>
  class exception_ai: public application_exception_base
  {
  public:

    exception_ai(size_t index):
      application_exception_base(string{type_tag<application_code>}, format(application_message.view(), " Answer with index \"", index, "\"."))
    {
    }
  };

  // Exception on a question and a transition. Reporting on the question's label and the destination question's label.
  template <tag_t application_code, tag_t application_message>
  class exception_qt: public application_exception_base
  {
  public:

    exception_qt(const string& question_label, const string& destination_question_label):
      application_exception_base(string{type_tag<application_code>}, format(application_message.view(), " At question with label \"", question_label, "\" and destination with label \"", destination_question_label, "\"."))
    {
    }
  };

  // Exception on a template question.
  template <tag_t application_code, tag_t application_message>
  class exception_tq: public application_exception_base
  {
  public:

    exception_tq(const string& question_label):
      application_exception_base(string{type_tag<application_code>}, format(application_message.view(), " Referring to template question with label \"", question_label, "\"."))
    {
    }
  };

  // Exceptions sorted by name in each group.

  // Function exceptions.
  using function_call_out_of_bounds = exception_q<"funcoob", "Function call's index is out of bounds. No corresponding function.">;
  using function_has_no_code = exception_q<"funcnoc", "Function has no code.">;
  using function_is_null = exception_q<"funcnull", "Function is null.">;
  using function_parameter_does_not_exist = exception_q<"funcpmiss", "Function parameter does not exist.">;
  using function_parameter_refers_to_question_with_different_loop_nest = exception_q<"funcprtqwdln", "Question begin loop refers to question with different loop nest.">;
  using function_parameter_refers_to_self = exception_q<"funcpself", "Function parameter refers to the question bearing it.">;
  using function_parameter_refers_to_subsequent_question = exception_q<"funcpsubseq", "Function parameter refers to a subsequent question.">;
  
  // Transitions exceptions.
  using transition_has_backwards_destination = exception_q<"trback", "Transition has backwards destination.">;
  using transition_has_both_condition_and_code = exception_q<"trhbcac", "Transition has both a condition and code specified.">;
  
  // Question exceptions.
  using question_begin_loop_has_no_operand = exception_q<"qblnoop", "Question begin loop has no operand.">;
  using question_begin_loop_refers_to_question_with_different_loop_nest = exception_q<"qblrtqwdln", "Question begin loop refers to question with different loop nest.">;
  using question_begin_loop_refers_to_unanswerable_question = exception_q<"qblrtaq", "Question begin loop refers to unanswerable question.">;
  using question_begin_loop_refers_to_unknown_question = exception_q<"qblrtuq", "Question begin loop refers to unknown question.">;
  using question_begin_loop_variable_is_invalid = exception_q<"qblvarinv", "Question begin loop has invalid variable.">;
  using question_is_orphan = exception_q<"qorphan", "Question is an orphan.">;
  using question_label_does_not_exist = exception_q<"qlnonexist", "Question label does not exist.">;
  using question_label_is_a_duplicate = exception_q<"qlabdup", "Question label is a duplicate.">;
  using question_label_is_invalid = exception_q<"qlabinv", "Question label is invalid.">;
  using question_loop_is_not_balanced = exception_q<"qlnotbal", "Question loop is not balanced.">;
  using question_loop_is_not_closed = exception_q<"qlnotcl", "Question loop is not closed.">;
  using question_loop_logic_error = exception_q<"qllerr", "Question loop logic error.">;
  using question_loop_variable_unknown = exception_q<"qlvarun", "Question loop variable unknown.">;
  using question_must_not_have_a_comment = exception_q<"qcommmiss", "Question must not have a comment.">;

  // Template question categories exceptions.
  using template_question_category_does_not_exist = exception<"tqcmiss", "Template question category does not exist.">;
  
  // Template question exceptions.
  using template_question_already_exists = exception<"tqexist", "A template question with that label already exists.">;
  using template_question_does_not_exist = exception<"tqmiss", "Template question does not exist.">;
  using template_question_does_not_exist_with_label = exception_tq<"tqmissl", "Template question does not exist.">;
  using template_question_is_invalid = exception<"tqinv", "Template question is invalid. It points at at a template.">;
  using template_question_language_is_invalid = exception<"tqlanginv", "Template question language is invalid.">;
  using template_question_misses_question = exception<"tqmissq", "Template question does not contain a question.">;
  
  // Template question localizations exceptions.
  using template_question_localization_contains_null_option = exception<"tqlnullo", "Template question localization has a null option.">;
  using template_question_localization_does_not_exist = exception<"tqlmiss", "Template question localization does not exist.">;
  using template_question_localization_already_exists = exception<"tqlaex", "Template question localization already exists.">;
  using template_question_localization_options_size_is_incorrect = exception<"tqloszinco", "Template question localization number of options localizations is incorrect.">;
  
  // Questionnaire exceptions.
  using questionnaire_does_not_exist = exception<"qqnonexist", "Questionnaire does not exist.">;
  using questionnaire_is_locked = exception<"qqlocked", "Questionnaire is locked, a campaign has been created.">;

  // Campaign exceptions.
  using campaign_does_not_exist = exception<"cmiss", "Campaign does not exist.">;
  using campaign_expired = exception<"cexp", "Campaign expired.">;
  using campaign_is_not_yet_active = exception<"cinact", "Campaign is not yet active.">;
  
  // Option localization exceptions.
  using option_localization_comment_does_not_exist = exception_q<"clcmiss", "Question option localization comment is missing.">;
  using option_localization_comment_is_present = exception_q<"clcpres", "Question option localization comment must not be supplied.">;
  using option_localization_label_is_empty = exception_q<"cllempt", "Question option localization's label is empty.">;
  
  // Question localization exceptions.
  using question_localization_contains_null_option = exception_q<"qlnullo", "Question has a null option.">;
  using question_localization_comment_is_missing = exception_q<"qlcmiss", "Question localization comment label localization is missing.">;
  using question_localization_comment_is_present = exception_q<"qlcpres", "Question localization comment label must not be supplied.">;
  using question_localization_for_template_does_not_exist = exception_q<"qlftmiss", "Question localization for template question is missing.">;
  using question_localization_is_duplicate = exception_q<"qldup", "Question localization is duplicated.">;
  using question_localization_does_not_exist = exception_q<"qlmiss", "Question localization is missing.">;
  using question_localization_option_duplicate = exception_q<"qlcdup", "Question localization options localizations contains a duplicate.">;
  using question_localization_options_size_is_incorrect = exception_q<"qloszinco", "Question localization number of options localizations is incorrect.">;
  using question_localization_text_is_missing = exception_q<"qltmiss", "Question localization text is missing.">;

  // Questionnaire localization exceptions.
  using questionnaire_localization_does_not_exist = exception<"qqlmiss", "Language not supported.">;
  using questionnaire_localization_in_english_does_not_exist = exception<"qqlengmiss", "Questionnaire localization in English does not exist.">;
  
  // Source question exceptions.
  using source_question_argument_does_not_exist = exception_qt<"sqanonex", "Source question argument does not exist.">;
  using source_question_begin_loop_transitions_to_begin_loop = exception_qt<"sqblttbl", "Source question begin loop transitions to another begin loop.">;
  using source_question_body_is_incorrect = exception_q<"sqbincorr", "Source question body is incorrect.">;
  using source_question_body_is_missing = exception_q<"sqbmiss", "Source question is missing a body.">;
  using source_question_contains_null_option = exception_q<"sqnullo", "Source question has a null option.">;
  using source_question_contains_null_transition = exception_q<"sqnullo", "Source question has a null transition.">;
  using source_question_from_template_has_a_body = exception_q<"sqfthasbody", "Source question from template has a body.">;
  using source_question_from_template_has_a_type = exception_q<"sqfthastype", "Source question from template has a type.">;
  using source_question_has_invalid_options = exception_q<"sqinvoptions", "Source question has invalid options.">;
  using source_question_text_is_missing = exception_q<"sqtextmiss", "Source question's text is missing.">;
  using source_question_transition_catch_all_is_not_last = exception_qt<"sqtcallnotl", "Source question has a transition with a catch-all and the transition is not the last one.">;
  using source_question_transition_condition_is_incorrect = exception_qt<"sqtcincorr", "Source question transition condition is incorrect.">;
  using source_question_transition_does_not_exist = exception_qt<"sqtnonex", "Source question transition does not exist.">;
  using source_question_transitions_across_loop = exception_qt<"sqtxloop", "Source question transitions across a loop.">;
  using source_question_transition_is_missing = exception_q<"sqtmiss", "Source question transition is missing.">;
  using source_question_transitions_lack_catch_all = exception_qt<"sqtlackcall", "Source question transitions lack a final catch-all.">;
  using source_question_transitions_to_itself = exception_q<"sqtself", "Source question transitions to itself.">;
  using source_question_transitions_to_previous_question = exception_qt<"sqtprev", "Source question transitions to previous question instead of subsequent.">;
  using source_question_type_is_invalid = exception_q<"sqtinv", "Source question type is invalid.">;
  using source_question_type_is_missing = exception_q<"sqtmiss", "Source question type is missing.">;

  // Source questionnaire exceptions.
  using source_questionnaire_contains_null_question = exception<"sqempty", "Source questionnaire has a null question.">;
  using source_questionnaire_has_no_questions = exception<"sqempty", "Source questionnaire has no questions.">;
  using source_questionnaire_name_is_empty = exception<"sqqname", "Source questionnaire name is empty.">;

  // Answers exceptions.
  // Nondescript exception thrown when the GUI has not done its validation job and is sending incorrect data. We do not need to be precise,
  // as this is the result of belt and suspenders check on the server side to doublecheck.
  using answer_index_does_not_exist = exception_ai<"aimiss", "Answer index does not exist.">;
  using answer_is_incorrect = exception<"abincorr", "Answer body is incorrect.">;
  using answer_is_missing = exception<"abmiss", "Answer body is missing.">;

  // Selections exceptions.
  using selection_is_invalid = exception<"sinv", "Selection is invalid.">;

  // Interview exceptions.
  using interview_does_not_exist = exception<"intmiss", "Interview does not exist.">;
  using interview_is_already_completed = exception<"intcompl", "Interview is already completed.">;
  using interview_is_already_started = exception<"intalst", "Interview is already started.">;
  using interview_is_not_started = exception<"intnotst", "Interview is not started.">;

  // Internal errors.
  using internal_error = exception<"ierr", "Internal error.">;

} // End namespace interviews.

#endif
