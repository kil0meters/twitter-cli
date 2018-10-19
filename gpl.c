#include <stdio.h>

#include "repl.h"

GenericObject *gpl_snippit(void) {
    printf("twitter-cli  Copyright (C) 2018 kilometers\n"
           "This program comes with ABSOLUTELY NO WARRANTY; for details type \'gpl warranty\'.\n"
           "This is free software, and you are welcome to redistribute it\n"
           "under certain conditions; type \'gpl conditions\' for details.)\n");
    return NULL;
}

GenericObject *gpl_terms_and_conditions(void) {
    printf("This program is free software: you can redistribute it and/or modify\n"
           "it under the terms of the GNU General Public License as published by\n"
           "the Free Software Foundation, either version 3 of the License, or\n"
           "(at your option) any later version.\n"
           "\n"
           "This program is distributed in the hope that it will be useful,\n"
           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           "GNU General Public License for more details.\n"
           "\n"
           "You should have received a copy of the GNU General Public License\n"
           "along with this program. If not, see <http://www.gnu.org/licenses/>.\n");
    return NULL;
}

GenericObject *gpl_warranty(void) {
    printf("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"
           "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"
           "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"
           "OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"
           "THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"
           "PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"
           "IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"
           "ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n");
    return NULL;
}