//
// Copyright Metaspex - 2023
// mailto:admin@metaspex.com
//

#ifndef HX2A_INTERVIEWS_MISC_HPP
#define HX2A_INTERVIEWS_MISC_HPP

#include <array>
#include <string>

#include "hx2a/json/value.hpp"
#include "hx2a/v8.hpp"
#include "hx2a/tag_type.hpp"

namespace interviews {

  using std::string;

  using namespace hx2a;

  template <tag_t tag>
  constexpr tag_t config_name = hx2a::tag_concat<"itv_", tag>;

  // elased and total_elapsed are part of the data of every answer. No need to add them here,
  constexpr char js_language_var[] = "language"; // The language code.
  constexpr char js_language_str2_var[] = "language_str2"; // The two-letter string.

  constexpr ::std::array reserved_labels = {js_language_var, js_language_str2_var};

  inline bool label_is_reserved(const string& label){
    for (const auto& res: reserved_labels){
      if (res == label){
	return true;
      }
    }

    return false;
  }

  // A function call in a parametric text is @{10}.
  constexpr char eval_prefix = '@';
  constexpr char eval_open = '{';
  constexpr char eval_close = '}';

  constexpr char javascript_library[] = R"( 
// Turns the first character, if any, to lowercase.
function itvFirstToLowerCase(a){ 
    if (Array.isArray(a)){
        let r = [];
        let i = 0;

        while (i < a.length){
            r.push(itvFirstToLowerCase(a[i]));
            ++i;
        }

        return r;
    }

    if ((typeof a !== 'string' && ! a instanceof String) || !a.length){
        return a;
    }

    return a[0].toLowerCase() + a.substring(1); 
} 

// Returns an array of the options' labels NOT selected in the choices indexes.
// For maximum reusability the functions are not specific to multiple choices question types. Up to the user to supply the path to the
// options and to the choices. The functions assume that the index key and the label key exist on choices and options array elements.
// The code below is quadratic. Not sure if we can do better in practice with small arrays...
function itvNotSelected(options, choices){ 
    if (options === undefined){
        return [];
    }

    if (choices === undefined){
        let i = 0; 
        let r = []; 

        // Looping through options.
        while (i < options.length){ 
            r.push(options[i].label); 
            ++i; 
        }

        return r;
    }

    if (!Array.isArray(options) || !Array.isArray(choices)){
        return [];
    }

    let i = 0; 
    let r = []; 

    // Looping through options.
    while (i < options.length){ 
        let j = 0; 

        // Looking if in choices the option has been chosen.
        while (j < choices.length){ 
            if (choices[j].index === i){
                break; 
            }

            ++j; 
        } 

        if (j === choices.length){
            r.push(options[i].label); 
        }

        ++i; 
    }

    return r; 
} 

// Removes the period at the end, if any.
function itvRemoveFinalPeriod(a){
    if (Array.isArray(a)){
        let r = [];
        let i = 0;

        while (i < a.length){
            r.push(itvRemoveFinalPeriod(a[i]));
            ++i;
        }

        return r;
    }

    if ((typeof a !== 'string' && ! a instanceof String) || !a.length){
        return a;
    }

    if (a[a.length - 1] === '.'){
        return a.substring(0, a.length - 1); 
    }

    return a;
} 

// Returns an array of the options' labels selected in the choices indexes.
function itvSelected(options, choices){
    if (options === undefined || choices === undefined || !Array.isArray(options) || !Array.isArray(choices)){
        return [];
    }

    let i = 0; 
    let r = []; 

    // Looping through options.
    while (i < options.length){ 
	let j = 0; 

	// Looking if in choices the option has been chosen.
	while (j < choices.length){ 
	    if (choices[j].index === i){ 
		r.push(options[i].label); 
		break; 
	    } 

	    ++j; 
	} 

	++i; 
    } 

    return r; 
}
)";
  
  constexpr char dbname[] = "idb";

  // Dummy returned value to be able to call initialize in a static variable in a function.
  bool initialize();

  // This calls initialize, use it.
  inline json_dom::value v8_execute(string_view s){
    [[maybe_unused]] static bool init = initialize();
    return v8::execute(s);
  }

  template <typename SlotJSType>
  inline json_dom::value slot_js_run(SlotJSType& sjs){
    [[maybe_unused]] static bool init = initialize();
    return sjs.run();
  }

} // End namespace interviews.

#endif
