#  Copyright (C) 2003-2025 Free Software Foundation, Inc.
#  Contributed by Kelley Cook, June 2004.
#  Original code from Neil Booth, May 2003.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3, or (at your option) any
# later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

# This Awk script reads in the option records generated from 
# opt-gather.awk, combines the flags of duplicate options and generates a
# C file.
#

# This program uses functions from opt-functions.awk and code from
# opt-read.awk.
#
# Usage: awk -f opt-functions.awk -f opt-read.awk -f optc-gen.awk \
#            [-v header_name=header.h] < inputfile > options.cc

# Dump that array of options into a C file.
END {


# Combine the flags of identical switches.  Switches
# appear many times if they are handled by many front
# ends, for example.
for (i = 0; i < n_opts; i++) {
    merged_flags[i] = flags[i]
}
for (i = 0; i < n_opts; i++) {
    while(i + 1 != n_opts && opts[i] == opts[i + 1] ) {
	merged_flags[i + 1] = merged_flags[i] " " merged_flags[i + 1];
	i++;
    }
}

# Record EnabledBy and LangEnabledBy uses.
n_enabledby = 0;
for (i = 0; i < n_langs; i++) {
    n_enabledby_lang[i] = 0;
}
for (i = 0; i < n_opts; i++) {
    enabledby_arg = opt_args("EnabledBy", flags[i]);
    if (enabledby_arg != "") {
        logical_and = index(enabledby_arg, " && ");
        if (logical_and != 0) {
            # EnabledBy(arg1 && arg2)
            split_sep = " && ";
        } else {
            # EnabledBy(arg) or EnabledBy(arg1 || arg2 || arg3)
            split_sep = " \\|\\| ";
        }
        n_enabledby_names = split(enabledby_arg, enabledby_names, split_sep);
        if (logical_and != 0 && n_enabledby_names > 2) {
            print "#error " opts[i] " EnabledBy(Wfoo && Wbar && Wbaz) currently not supported"
        }
        for (j = 1; j <= n_enabledby_names; j++) {
            enabledby_name = enabledby_names[j];
            enabledby_index = opt_numbers[enabledby_name];
            if (enabledby_index == "") {
                print "#error " opts[i] " Enabledby(" enabledby_name "), unknown option '" enabledby_name "'"
            } else if (!flag_set_p("Common", merged_flags[enabledby_index])) {
		print "#error " opts[i] " Enabledby(" enabledby_name "), '" \
		    enabledby_name "' must have flag 'Common'"		\
		    " to use Enabledby(), otherwise use LangEnabledBy()"
	    } else {
		condition = "";
                if (logical_and != 0) {
                    opt_var_name_1 = search_var_name(enabledby_names[1], opt_numbers, opts, flags, n_opts);
                    opt_var_name_2 = search_var_name(enabledby_names[2], opt_numbers, opts, flags, n_opts);
                    if (opt_var_name_1 == "") {
                        print "#error " enabledby_names[1] " does not have a Var() flag"
                    }
                    if (opt_var_name_2 == "") {
                        print "#error " enabledby_names[2] " does not have a Var() flag"
                    }
                    condition = "opts->x_" opt_var_name_1 " && opts->x_" opt_var_name_2;
                }
                if (enables[enabledby_name] == "") {
                    enabledby[n_enabledby] = enabledby_name;
                    n_enabledby++;
                }
                enables[enabledby_name] = enables[enabledby_name] opts[i] ";";
                enablesif[enabledby_name] = enablesif[enabledby_name] condition ";";
            }
        }
    }

    enabledby_arg = opt_args("LangEnabledBy", flags[i]);
    if (enabledby_arg != "") {
	enabledby_n_args = n_args(enabledby_arg)
	if (enabledby_n_args != 2 \
	    && enabledby_n_args != 4) {
	    print "#error " opts[i] " LangEnabledBy(" enabledby_arg ") must specify two or four arguments"
	}

	enabledby_langs = nth_arg(0, enabledby_arg);
	if (enabledby_langs == "")
	    print "#error " opts[i] " LangEnabledBy(" enabledby_arg ") must specify LANGUAGE"
	enabledby_opt = nth_arg(1, enabledby_arg);
	if (enabledby_opt == "")
	    print "#error " opts[i] " LangEnabledBy(" enabledby_arg ") must specify OPT"

	enabledby_posarg_negarg = ""
	if (enabledby_n_args == 4) {
	    enabledby_posarg = nth_arg(2, enabledby_arg);
	    enabledby_negarg = nth_arg(3, enabledby_arg);
	    if (enabledby_posarg == "" \
		|| enabledby_negarg == "")
		print "#error " opts[i] " LangEnabledBy(" enabledby_arg ") with four arguments must specify POSARG and NEGARG"
	    else
		enabledby_posarg_negarg = "," enabledby_posarg "," enabledby_negarg
	}

	n_enabledby_arg_langs = split(enabledby_langs, enabledby_arg_langs, " ");
	n_enabledby_array = split(enabledby_opt, enabledby_array, " \\|\\| ");
	for (k = 1; k <= n_enabledby_array; k++) {
	    enabledby_index = opt_numbers[enabledby_array[k]];
	    if (enabledby_index == "") {
		print "#error " opts[i] " LangEnabledBy(" enabledby_arg "), unknown option '" enabledby_opt "'"
		continue
	    }

	    for (j = 1; j <= n_enabledby_arg_langs; j++) {
		lang_name = enabledby_arg_langs[j]
		lang_index = lang_numbers[lang_name];
		if (lang_index == "") {
		    print "#error " opts[i] " LangEnabledBy(" enabledby_arg "), unknown language '" lang_name "'"
		    continue
		}

		lang_name = lang_sanitized_name(lang_name);

		if (enables[lang_name,enabledby_array[k]] == "") {
		    enabledby[lang_name,n_enabledby_lang[lang_index]] = enabledby_array[k];
		    n_enabledby_lang[lang_index]++;
		}
		enables[lang_name,enabledby_array[k]] \
		    = enables[lang_name,enabledby_array[k]] opts[i] enabledby_posarg_negarg ";";
	    }
	}
    }

    if (flag_set_p("Param", flags[i]) && !(opts[i] ~ "^-param="))
      print "#error Parameter option name '" opts[i] "' must start with '-param='"
}


print "/* This file is auto-generated by optc-gen.awk.  */"
print ""
print "#define INCLUDE_MEMORY"
n_headers = split(header_name, headers, " ")
for (i = 1; i <= n_headers; i++)
	print "#include " quote headers[i] quote
print "#include " quote "opts.h" quote
print "#include " quote "intl.h" quote
print "#include " quote "insn-attr-common.h" quote
print ""

if (n_extra_c_includes > 0) {
	for (i = 0; i < n_extra_c_includes; i++) {
		print "#include " quote extra_c_includes[i] quote
	}
	print ""
}

for (i = 0; i < n_enums; i++) {
	name = enum_names[i]
	type = enum_type[name]
	print "static const struct cl_enum_arg cl_enum_" name \
	    "_data[] = "
	print "{"
	print enum_data[name] "  { NULL, 0, 0 }"
	print "};"
	print ""
	print "static void"
	print "cl_enum_" name "_set (void *var, int value)"
	print "{"
	print "  *((" type " *) var) = (" type ") value;"
	print "}"
	print ""
	print "static int"
	print "cl_enum_" name "_get (const void *var)"
	print "{"
	print "  return (int) *((const " type " *) var);"
	print "}"
	print ""
}

print "const struct cl_enum cl_enums[] ="
print "{"
for (i = 0; i < n_enums; i++) {
	name = enum_names[i]
	ehelp = enum_help[name]
	if (ehelp == "")
		ehelp = "NULL"
	else
		ehelp = quote ehelp quote
	unknown_error = enum_unknown_error[name]
	if (unknown_error == "")
		unknown_error = "NULL"
	else
		unknown_error = quote unknown_error quote
	print "  {"
	print "    " ehelp ","
	print "    " unknown_error ","
	print "    cl_enum_" name "_data,"
	print "    sizeof (" enum_type[name] "),"
	print "    cl_enum_" name "_set,"
	print "    cl_enum_" name "_get"
	print "  },"
}
print "};"
print "const unsigned int cl_enums_count = " n_enums ";"
print ""

print "const struct gcc_options global_options_init =\n{"
for (i = 0; i < n_extra_vars; i++) {
	var = extra_vars[i]
	init = extra_vars[i]
	if (var ~ "=" ) {
		sub(".*= *", "", init)
	} else {
		init = "0"
	}
	sub(" *=.*", "", var)
	name = var
	sub("^.*[ *]", "", name)
	sub("\\[.*\\]$", "", name)
	var_seen[name] = 1
	print "  " init ", /* " name " */"
}
for (i = 0; i < n_opts; i++) {
	name = var_name(flags[i]);
	init = opt_args("Init", flags[i])

	if (name == "") {
		if (init != "")
		    print "#error " opts[i] " must specify Var to use Init"
		continue;
	}

	if (init != "") {
		if (name in var_init && var_init[name] != init)
			print "#error multiple initializers for " name
		var_init[name] = init
	}
}
for (i = 0; i < n_opts; i++) {
	name = var_name(flags[i]);
	if (name == "")
		continue;

	if (name in var_seen)
		continue;

	if (name in var_init)
		init = var_init[name]
	else
		init = "0"

	print "  " init ", /* " name " */"

	var_seen[name] = 1;
}
for (i = 0; i < n_opts; i++) {
	name = static_var(opts[i], flags[i]);
	if (name != "") {
		print "  0, /* " name " (private state) */"
		print "#undef x_" name
	}
}
for (i = 0; i < n_opts; i++) {
	if (flag_set_p("SetByCombined", flags[i]))
		print "  false, /* frontend_set_" var_name(flags[i]) " */"
}
print "};"
print ""
print "struct gcc_options global_options;"
print "struct gcc_options global_options_set;"
print ""

print "const char * const lang_names[] =\n{"
for (i = 0; i < n_langs; i++) {
        macros[i] = "CL_" lang_sanitized_name(langs[i])
	s = substr("         ", length (macros[i]))
	print "  " quote langs[i] quote ","
    }

print "  0\n};\n"
print "const unsigned int cl_options_count = N_OPTS;\n"
print "#if (1U << " n_langs ") > CL_MIN_OPTION_CLASS"
print "  #error the number of languages exceeds the implementation limit"
print "#endif"
print "const unsigned int cl_lang_count = " n_langs ";\n"

print "const struct cl_option cl_options[] =\n{"

j = 0
for (i = 0; i < n_opts; i++) {
	back_chain[i] = "N_OPTS";
	indices[opts[i]] = j;
	# Combine the flags of identical switches.  Switches
	# appear many times if they are handled by many front
	# ends, for example.
	while( i + 1 != n_opts && opts[i] == opts[i + 1] ) {
		flags[i + 1] = flags[i] " " flags[i + 1];
		if (help[i + 1] == "")
			help[i + 1] = help[i]
		else if (help[i] != "" && help[i + 1] != help[i]) {
			print "#error Multiple different help strings for " \
				opts[i] ":"
			print "#error   " help[i]
			print "#error   " help[i + 1]
		}
				
		i++;
		back_chain[i] = "N_OPTS";
		indices[opts[i]] = j;
	}
	j++;
}

optindex = 0
for (i = 0; i < n_opts; i++) {
	# With identical flags, pick only the last one.  The
	# earlier loop ensured that it has all flags merged,
	# and a nonempty help text if one of the texts was nonempty.
	while( i + 1 != n_opts && opts[i] == opts[i + 1] ) {
		i++;
	}

	len = length (opts[i]);
	enum = opt_enum(opts[i])

	# Do not allow Joined and Separate properties if
	# an options ends with '='.
	if (flag_set_p("Joined", flags[i]) && flag_set_p("Separate", flags[i]) && opts[i] ~ "=$") {
		print "#error Option '" opts[i] "' ending with '=' cannot have " \
			"both Joined and Separate properties"
	}

	# If this switch takes joined arguments, back-chain all
	# subsequent switches to it for which it is a prefix.  If
	# a later switch S is a longer prefix of a switch T, T
	# will be back-chained to S in a later iteration of this
	# for() loop, which is what we want.
	if (flag_set_p("Joined.*", flags[i])) {
		for (j = i + 1; j < n_opts; j++) {
			if (substr (opts[j], 1, len) != opts[i])
				break;
			back_chain[j] = enum;
		}
	}

	s = substr("                                  ", length (opts[i]))
	if (i + 1 == n_opts)
		comma = ""

	if (help[i] == "")
		hlp = "NULL"
	else
		hlp = quote help[i] quote;

	missing_arg_error = opt_args("MissingArgError", flags[i])
	if (missing_arg_error == "")
		missing_arg_error = "NULL"
	else
		missing_arg_error = quote missing_arg_error quote


	warn_message = opt_args("Warn", flags[i])
	if (warn_message == "")
		warn_message = "NULL"
	else
		warn_message = quote warn_message quote

	alias_arg = opt_args("Alias", flags[i])
	if (alias_arg == "") {
		if (flag_set_p("Ignore", flags[i])) {
			  alias_data = "NULL, NULL, OPT_SPECIAL_ignore"
        if (warn_message != "NULL")
				  print "#error Ignored option with Warn"
        if (var_name(flags[i]) != "")
				  print "#error Ignored option with Var"
      }
    else if (flag_set_p("Deprecated", flags[i]))
        print "#error Deprecated was replaced with WarnRemoved"
    else if (flag_set_p("WarnRemoved", flags[i])) {
			  alias_data = "NULL, NULL, OPT_SPECIAL_warn_removed"
        if (warn_message != "NULL")
				  print "#error WarnRemoved option with Warn"
      }
		else
			alias_data = "NULL, NULL, N_OPTS"
		if (flag_set_p("Enum.*", flags[i])) {
			if (!flag_set_p("RejectNegative", flags[i]) \
			    && !flag_set_p("EnumSet", flags[i]) \
			    && !flag_set_p("EnumBitSet", flags[i]) \
			    && opts[i] ~ "^[Wfgm]")
				print "#error Enum allowing negative form"
		}
	} else {
		alias_opt = nth_arg(0, alias_arg)
		alias_posarg = nth_arg(1, alias_arg)
		alias_negarg = nth_arg(2, alias_arg)

		if (var_ref(opts[i], flags[i]) != "(unsigned short) -1")
			print "#error Alias setting variable"

		if (alias_posarg != "" && alias_negarg == "") {
			if (!flag_set_p("RejectNegative", flags[i]) \
			    && opts[i] ~ "^[Wfm]")
				print "#error Alias with single argument " \
					"allowing negative form"
		}
		if (alias_posarg != "" \
		    && flag_set_p("NegativeAlias", flags[i])) {
			print "#error Alias with multiple arguments " \
				"used with NegativeAlias"
		}

		alias_opt = opt_enum(alias_opt)
		if (alias_posarg == "")
			alias_posarg = "NULL"
		else
			alias_posarg = quote alias_posarg quote
		if (alias_negarg == "")
			alias_negarg = "NULL"
		else
			alias_negarg = quote alias_negarg quote
		alias_data = alias_posarg ", " alias_negarg ", " alias_opt
	}

	neg = opt_args("Negative", flags[i]);
	if (neg != "")
		idx = indices[neg]
	else {
		if (flag_set_p("RejectNegative", flags[i]))
			idx = -1;
		else {
			if (opts[i] ~ "^[Wfgm]")
				idx = indices[opts[i]];
			else
				idx = -1;
		}
	}
	# Split the printf after %u to work around an ia64-hp-hpux11.23
	# awk bug.
	printf(" /* [%i] = */ {\n", optindex)
	printf("    %c-%s%c,\n    %s,\n    %s,\n    %s,\n    %s, %s, %u,",
	       quote, opts[i], quote, hlp, missing_arg_error, warn_message,
	       alias_data, back_chain[i], len)
	printf(" /* .neg_idx = */ %d,\n", idx)
	condition = opt_args("Condition", flags[i])
	cl_flags = switch_flags(flags[i])
	cl_bit_fields = switch_bit_fields(flags[i])
	cl_zero_bit_fields = switch_bit_fields("")
	if (condition != "")
		printf("#if %s\n" \
		       "    %s,\n" \
		       "    0, %s,\n" \
		       "#else\n" \
		       "    0,\n" \
		       "    1 /* Disabled.  */, %s,\n" \
		       "#endif\n",
		       condition, cl_flags, cl_bit_fields, cl_zero_bit_fields)
	else
		printf("    %s,\n" \
		       "    0, %s,\n",
		       cl_flags, cl_bit_fields)
	printf("    %s, %s, %s }%s\n", var_ref(opts[i], flags[i]),
	       var_set(flags[i]), integer_range_info(opt_args("IntegerRange", flags[i]),
		    opt_args("Init", flags[i]), opts[i], flag_set_p("UInteger", flags[i])), comma)

	# Bump up the informational option index.
	++optindex
 }

print "};"

print "\n\n"
print "bool                                                                  "
print "common_handle_option_auto (struct gcc_options *opts,                  "
print "                           struct gcc_options *opts_set,              "
print "                           const struct cl_decoded_option *decoded,   "
print "                           unsigned int lang_mask, int kind,          "
print "                           location_t loc,                            "
print "                           const struct cl_option_handlers *handlers, "
print "                           diagnostics::context *dc)                  "
print "{                                                                     "
print "  size_t scode = decoded->opt_index;                                  "
print "  HOST_WIDE_INT value = decoded->value;                               "
print "  enum opt_code code = (enum opt_code) scode;                         "
print "                                                                      "
print "  gcc_assert (decoded->canonical_option_num_elements <= 2);           "
print "                                                                      "
print "  switch (code)                                                       "
print "    {                                                                 "
# Handle EnabledBy
for (i = 0; i < n_enabledby; i++) {
    enabledby_name = enabledby[i];
    print "    case " opt_enum(enabledby_name) ":"
    n_enables = split(enables[enabledby_name], thisenable, ";");
    n_enablesif = split(enablesif[enabledby_name], thisenableif, ";");
    if (n_enables != n_enablesif) {
        print "#error n_enables != n_enablesif: Something went wrong!"
    }
    for (j = 1; j < n_enables; j++) {
        opt_var_name = var_name(flags[opt_numbers[thisenable[j]]]);
        if (opt_var_name != "") {
            condition = "!opts_set->x_" opt_var_name
            if (thisenableif[j] != "") {
                value = "(" thisenableif[j] ")"
            } else {
                value = "value"
            }
            print "      if (" condition ")"
            print "        handle_generated_option (opts, opts_set,"
            print "                                 " opt_enum(thisenable[j]) ", NULL, " value ","
            print "                                 lang_mask, kind, loc, handlers, true, dc);"
        } else {
            print "#error " thisenable[j] " does not have a Var() flag"
        }
    }
    print "      break;\n"
}
print "    default:    "
print "      break;    "
print "    }           "
print "  return true;  "
print "}               "

# Handle LangEnabledBy
for (i = 0; i < n_langs; i++) {
    lang_name = lang_sanitized_name(langs[i]);
    mark_unused = " ATTRIBUTE_UNUSED";

    print "\n\n"
    print "bool                                                                  "
    print lang_name "_handle_option_auto (struct gcc_options *opts" mark_unused ",              "
    print "                           struct gcc_options *opts_set" mark_unused ",              "
    print "                           size_t scode" mark_unused ", const char *arg" mark_unused ", HOST_WIDE_INT value" mark_unused ",  "
    print "                           unsigned int lang_mask" mark_unused ", int kind" mark_unused ",          "
    print "                           location_t loc" mark_unused ",                            "
    print "                           const struct cl_option_handlers *handlers" mark_unused ", "
    print "                           diagnostics::context *dc" mark_unused ")                  "
    print "{                                                                     "
    print "  enum opt_code code = (enum opt_code) scode;                         "
    print "                                                                      "
    print "  switch (code)                                                       "
    print "    {                                                                 "
    
    for (k = 0; k < n_enabledby_lang[i]; k++) {
        enabledby_name = enabledby[lang_name,k];
        print "    case " opt_enum(enabledby_name) ":"
        n_thisenable = split(enables[lang_name,enabledby_name], thisenable, ";");
        for (j = 1; j < n_thisenable; j++) {
            n_thisenable_args = split(thisenable[j], thisenable_args, ",");
            if (n_thisenable_args == 1) {
                thisenable_opt = thisenable[j];
                value = "value";
            } else {
                thisenable_opt = thisenable_args[1];
                with_posarg = thisenable_args[2];
                with_negarg = thisenable_args[3];
                value = "value ? " with_posarg " : " with_negarg;
            }
            opt_var_name = var_name(flags[opt_numbers[thisenable_opt]]);
            if (opt_var_name != "") {
                print "      if (!opts_set->x_" opt_var_name ")"
                print "        handle_generated_option (opts, opts_set,"
                print "                                 " opt_enum(thisenable_opt) ", NULL, " value ","
                print "                                 lang_mask, kind, loc, handlers, true, dc);"
            } else {
                print "#error " thisenable_opt " does not have a Var() flag"
            }
        }
        print "      break;\n"
    }
    print "    default:    "
    print "      break;    "
    print "    }           "
    print "  return true;  "
    print "}               "
}

#Handle CPP()
print "\n"
print "#include " quote "cpplib.h" quote;
print "void"
print "cpp_handle_option_auto (const struct gcc_options * opts,                   "
print "                        size_t scode, struct cpp_options * cpp_opts)"    
print "{                                                                     "
print "  enum opt_code code = (enum opt_code) scode;                         "
print "                                                                      "
print "  switch (code)                                                       "
print "    {                                                                 "
for (i = 0; i < n_opts; i++) {
    # With identical flags, pick only the last one.  The
    # earlier loop ensured that it has all flags merged,
    # and a nonempty help text if one of the texts was nonempty.
    while( i + 1 != n_opts && opts[i] == opts[i + 1] ) {
        i++;
    }

    cpp_option = nth_arg(0, opt_args("CPP", flags[i]));
    if (cpp_option != "") {
        opt_var_name = var_name(flags[i]);
        init = opt_args("Init", flags[i])
        if (opt_var_name != "" && init != "") {
            print "    case " opt_enum(opts[i]) ":"
            print "      cpp_opts->" cpp_option " = opts->x_" opt_var_name ";"
            print "      break;"
        } else if (opt_var_name == "" && init == "") {
            print "#error CPP() requires setting Init() and Var() for " opts[i]
        } else if (opt_var_name != "") {
            print "#error CPP() requires setting Init() for " opts[i]
        } else {
            print "#error CPP() requires setting Var() for " opts[i]
        }
    }
}
print "    default:    "
print "      break;    "
print "    }           "
print "}\n"
print "void"
print "init_global_opts_from_cpp(struct gcc_options * opts,                   "
print "                         const struct cpp_options * cpp_opts)"    
print "{                                                                     "
for (i = 0; i < n_opts; i++) {
    # With identical flags, pick only the last one.  The
    # earlier loop ensured that it has all flags merged,
    # and a nonempty help text if one of the texts was nonempty.
    while( i + 1 != n_opts && opts[i] == opts[i + 1] ) {
        i++;
    }
    cpp_option = nth_arg(0, opt_args("CPP", flags[i]));
    opt_var_name = var_name(flags[i]);
    if (cpp_option != "" && opt_var_name != "") {
        print "  opts->x_" opt_var_name " = cpp_opts->" cpp_option ";"
    }
}
print "}               "

split("", var_seen, ":")
print "\n#if !defined(GENERATOR_FILE) && defined(ENABLE_PLUGIN)"
print "DEBUG_VARIABLE const struct cl_var cl_vars[] =\n{"

for (i = 0; i < n_opts; i++) {
	name = var_name(flags[i]);
	if (name == "")
		continue;
	var_seen[name] = 1;
}

for (i = 0; i < n_extra_vars; i++) {
	var = extra_vars[i]
	sub(" *=.*", "", var)
	name = var
	sub("^.*[ *]", "", name)
	sub("\\[.*\\]$", "", name)
	if (name in var_seen)
		continue;
	print "  { " quote name quote ", offsetof (struct gcc_options, x_" name ") },"
	var_seen[name] = 1
}

print "  { NULL, (unsigned short) -1 }\n};\n#endif"

}
