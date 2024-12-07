/* Test pass
   Vladyslav Savhcyn, 2024-11 
   Modelled on tree-nrv.cc

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define INCLUDE_MEMORY
#include "config.h" /* One of the main headers that are loaded 
                     * into GCC source files. 
                     * Contains system-specific configuration details 
                     * determined during the build process */
#include "system.h" /* Provides a standardized way to include 
                     * sys and std headers */
#include "coretypes.h" /* This one contains fundamental type definitions and macros. */
#include "backend.h" /* Main header for compiler's backend infrastructure. */
#include "tree.h" /* Defines tree data structure, a GCC's primary internal representation
                   * of program structure. 
                   * This is one of our main tools for seeing and 
                   * using GCC representation and transformations of source code. */
#include "gimple.h" /* Defines GIMPLE intermediate representations.
                     * We will use it with the previous header to prune our code later. */
#include "tree-pass.h" /* A header that defines infrastructure for compiler passes. 
                        * Meaning, that this is our core header */
#include "ssa.h" /* A header that implements the Static Single Assignment (SSA) form. 
                  * It may contain something usefull for later iterations */
#include "gimple-iterator.h" /* Iterators for GIMPLE statements. 
                              * Safe and convenient iteration over basic blocks and statements */
#include "gimple-walk.h" /* Tools for traversing and analyzing GIMPLE representation.
                          * It might be useful in later iterations */
#include "internal-fn.h" /* Defines compiler-specific functions? */
#include "gimple-pretty-print.h" /* Functions for printing GIMPLE representations in a human-readable format. */

// Added headers:
#include "gimple-ssa.h"
#include "cgraph.h"
#include "attribs.h"
#include "pretty-print.h"
#include "tree-inline.h"
#include "intl.h"

// =================================================== vvv
// Test pass

namespace {
    const pass_data pass_data_prune = {
        GIMPLE_PASS,   /* type */
        "prune",       /* name */
        OPTGROUP_NONE, /* optinfo_flags */
        TV_NONE,       /* tv_id */
        PROP_cfg,      /* properties_required */
        0,             /* properties_provided */
        0,             /* properties_destroyed */
        0,             /* todo_flags_start */
        0,             /* todo_flags_finish */
    };

    class pass_prune : public gimple_opt_pass {
    public:
        pass_prune(gcc::context* ctxt)
            : gimple_opt_pass(pass_data_prune, ctxt) {}

        /* opt_pass methods: */
        bool gate(function*) final override {
            return 1;  // always execute pass
        }

        unsigned int execute(function* fun) final override;

    };  // class pass_prune

    unsigned int pass_prune::execute(function* fun) {
        unsigned total_stmt_count = 0;
        unsigned iter_stmt = 0;
        int* stmt_buffer_ints = NULL;
        basic_block bb;

        /*
         * Good for checking if the we have different clones of this function
        tree fndecl = fun->decl;
        tree target_clone_attr = lookup_attribute("target_clones", DECL_ATTRIBUTES(fndecl));
    
        if (target_clone_attr) {
            // Extract clone names from the attribute
            tree clone_names = TREE_VALUE(target_clone_attr);
            // Process clone_names to get specific clone types
            
        }
        */

        if (dump_file) {
            fprintf(dump_file,
                    "\n\n#### tree_prune.cc: begin of prunning diagnostics "
                    "####\n\n\n");
        }

        FOR_EACH_BB_FN(bb, fun) {
            gimple_stmt_iterator gsi;
            for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                total_stmt_count++;
            }
        }

        stmt_buffer_ints = new int[total_stmt_count];
        if (!stmt_buffer_ints) {
            return 0;
        }

        memset(stmt_buffer_ints, 0, (total_stmt_count) * sizeof(int));

        FOR_EACH_BB_FN(bb, fun) {
            gimple_stmt_iterator gsi;
            for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi);
                 gsi_next(&gsi), iter_stmt++) {
                gimple* stmt = gsi_stmt(gsi);
                stmt_buffer_ints[iter_stmt] = stmt->code;
                // print_gimple_stmt(dump_file, stmt, 0, TDF_DETAILS);
            }
        }

        if (dump_file) {
            fprintf(dump_file, "Number of statements: %d\n", total_stmt_count);
            fprintf(dump_file, "Statement count string:\n");

            for (iter_stmt = 0; iter_stmt < total_stmt_count; iter_stmt++) {
                fprintf(dump_file, "%d", stmt_buffer_ints[iter_stmt]);
            }

            fprintf(dump_file,
                    "\n\n#### tree_prune.cc: end of prune diagnostics, start "
                    "regular dump of current gimple ####\n\n\n");
        }

        delete[] stmt_buffer_ints;
        return 0;
    }

}  // namespace

gimple_opt_pass* make_pass_prune(gcc::context* ctxt) {
    return new pass_prune(ctxt);
}

// =================================================== ^^^
