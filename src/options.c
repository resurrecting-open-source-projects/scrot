/* options.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2008      William Vera <billy@billy.com.mx>
Copyright 2009      George Danchev <danchev@spnet.net>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019      Daniel Lublin <daniel@lublin.se>
Copyright 2019      Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2019      Jade Auer <jade@trashwitch.dev>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "scrot.h"
#include "options.h"

static void scrot_parse_option_array(int argc, char **argv);
scrotoptions opt;

void
init_parse_options(int argc, char **argv)
{
   /* Set default options */
   memset(&opt, 0, sizeof(scrotoptions));

   opt.quality = 75;
   opt.overwrite = 0;
   opt.line_width = 1;
   opt.line_color = "gray";

   /* Parse the cmdline args */
   scrot_parse_option_array(argc, argv);
}

int
options_parse_required_number(char *str)
{
   char *end = NULL;
   int ret = 0;
   errno = 0;

   ret = strtol(str, &end, 10);

   if (str == end) {
      fprintf(stderr, "the option is not a number: %s\n", end);
      exit(EXIT_FAILURE);
   }

   if (errno) {
      perror("strtol");
      exit(EXIT_FAILURE);
   }

   return ret;
}

static void
options_parse_line(char *optarg)
{
   enum {WIDTH=0, COLOR };

   char *const token[] = {
      [WIDTH] = "width",
      [COLOR] = "color",
      NULL
   };

   char *subopts = optarg;
   char *value = NULL;

   while (*subopts != '\0') {
      switch(getsubopt(&subopts, token, &value)) {
         case WIDTH:

            if (value == NULL) {
               fprintf(stderr, "Missing value for "
                     "suboption '%s'\n", token[WIDTH]);
               exit(EXIT_FAILURE);
            }

            opt.line_width = options_parse_required_number(value);

            if (opt.line_width <= 0 || opt.line_width > 8){
               fprintf(stderr, "Value of the range (1..8) for "
                     "suboption '%s': %d\n", token[WIDTH], opt.line_width);
               exit(EXIT_FAILURE);
            }
            break;

         case COLOR:

            if (value == NULL || *value == '\0') {
               fprintf(stderr, "Missing value for "
                     "suboption '%s'\n", token[COLOR]);
               exit(EXIT_FAILURE);
            }

            opt.line_color = strdup(value);

            break;
         default:
            fprintf(stderr, "No match found for token: '%s'\n", value);
            exit(EXIT_FAILURE);
            break;
      }

   } /* while */
}


static void
scrot_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "a:opbcd:e:hmq:st:uv+:zn:l:";

   static struct option lopts[] = {
      /* actions */
      {"help", 0, 0, 'h'},                  /* okay */
      {"version", 0, 0, 'v'},               /* okay */
      {"count", 0, 0, 'c'},
      {"select", 0, 0, 's'},
      {"focused", 0, 0, 'u'},
      {"focussed", 0, 0, 'u'},	/* macquarie dictionary has both spellings */
      {"border", 0, 0, 'b'},
      {"multidisp", 0, 0, 'm'},
      {"silent", 0, 0, 'z'},
      {"pointer", 0, 0, 'p'},
      {"overwrite", 0, 0, 'o'},
      /* toggles */
      {"thumb", 1, 0, 't'},
      {"delay", 1, 0, 'd'},
      {"quality", 1, 0, 'q'},
      {"exec", 1, 0, 'e'},
      {"debug-level", 1, 0, '+'},
      {"autoselect", required_argument, 0, 'a'},
      {"note", required_argument, 0, 'n'},
      {"line", required_argument, 0, 'l'},
      {0, 0, 0, 0}
   };
   int optch = 0, cmdx = 0;

   /* Now to pass some optionarinos */
   while ((optch = getopt_long(argc, argv, stropts, lopts, &cmdx)) !=
          EOF)
   {
      switch (optch)
      {
        case 0:
           break;
        case 'h':
           show_usage();
           break;
        case 'v':
           show_version();
           break;
        case 'b':
           opt.border = 1;
           break;
        case 'd':
           opt.delay = options_parse_required_number(optarg);
           break;
        case 'e':
           opt.exec = gib_estrdup(optarg);
           break;
        case 'm':
           opt.multidisp = 1;
           break;
        case 'q':
           opt.quality = options_parse_required_number(optarg);
           break;
        case 's':
           opt.select = 1;
           break;
        case 'u':
           opt.focused = 1;
           break;
        case '+':
           opt.debug_level = options_parse_required_number(optarg);
           break;
        case 'c':
           opt.countdown = 1;
           break;
        case 't':
           options_parse_thumbnail(optarg);
           break;
        case 'z':
           opt.silent = 1;
           break;
        case 'p':
           opt.pointer = 1;
           break;
        case 'o':
           opt.overwrite = 1;
           break;
        case 'a':
           options_parse_autoselect(optarg);
           break;
        case 'n':
           options_parse_note(optarg);
           break;
        case 'l':
           options_parse_line(optarg);
           break;
        case '?':
           exit(EXIT_FAILURE);
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
   if (optind < argc)
   {
      while (optind < argc)
      {
         /* If recursive is NOT set, but the only argument is a directory
            name, we grab all the files in there, but not subdirs */
         if (!opt.output_file)
         {
            opt.output_file = argv[optind++];

            if ( strlen(opt.output_file) > 256 ) {
               printf("output filename too long.\n");
               exit(EXIT_FAILURE);
            }

            if (opt.thumb)
               opt.thumb_file = name_thumbnail(opt.output_file);
         }
         else
            gib_weprintf("unrecognised option %s\n", argv[optind++]);
      }
   }

   /* So that we can safely be called again */
   optind = 1;
}

char *
name_thumbnail(char *name)
{
   size_t length = 0;
   char *new_title;
   char *dot_pos;
   size_t diff = 0;

   length = strlen(name) + 7;
   new_title = gib_emalloc(length);

   dot_pos = strrchr(name, '.');
   if (dot_pos)
   {
      diff = (dot_pos - name) / sizeof(char);

      strncpy(new_title, name, diff);
      strcat(new_title, "-thumb");
      strcat(new_title, dot_pos);
   }
   else
      sprintf(new_title, "%s-thumb", name);

   return new_title;
}

void
options_parse_autoselect(char *optarg)
{
   char *tok;
   const char tokdelim[2] = ",";
   int dimensions[4];
   int i=0;

   if (strchr(optarg, ',')) /* geometry dimensions must be in format x,y,w,h   */
   {
     dimensions[i++] = options_parse_required_number(strtok(optarg, tokdelim));
     while ((tok = strtok(NULL, tokdelim)) )
        dimensions[i++] = options_parse_required_number(tok);
     opt.autoselect=1;
     opt.autoselect_x=dimensions[0];
     opt.autoselect_y=dimensions[1];
     opt.autoselect_w=dimensions[2];
     opt.autoselect_h=dimensions[3];
   }
}

void
options_parse_thumbnail(char *optarg)
{
   char *tok;

   if (strchr(optarg, 'x')) /* We want to specify the geometry */
   {
     tok = strtok(optarg, "x");
     opt.thumb_width = options_parse_required_number(tok);
     tok = strtok(NULL, "x");
     if (tok)
     {
       opt.thumb_width = options_parse_required_number(optarg);
       opt.thumb_height = options_parse_required_number(tok);

       if (opt.thumb_width < 0)
         opt.thumb_width = 1;
       if (opt.thumb_height < 0)
         opt.thumb_height = 1;

       if (!opt.thumb_width && !opt.thumb_height)
         opt.thumb = 0;
       else
         opt.thumb = 1;
     }
   }
   else
   {
     opt.thumb = options_parse_required_number(optarg);
     if (opt.thumb < 1)
       opt.thumb = 1;
     else if (opt.thumb > 100)
       opt.thumb = 100;
   }
}

void options_parse_note(char *optarg)
{
   opt.note = gib_estrdup(optarg);

   if (opt.note == NULL) return;

   if (opt.note[0] == '\0') {
      fprintf(stderr, "Required arguments for --note.");
      exit(EXIT_FAILURE);
   }

   scrot_note_new(opt.note);
}

void
show_version(void)
{
   printf(SCROT_PACKAGE " version " SCROT_VERSION "\n");
   exit(0);
}

void
show_mini_usage(void)
{
   printf("Usage : " SCROT_PACKAGE " [OPTIONS]... FILE\nUse " SCROT_PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


void
show_usage(void)
{
   fprintf(stdout,
           "Usage : " SCROT_PACKAGE " [OPTIONS]... [FILE]\n"
           "  Where FILE is the target file for the screenshot.\n"
           "  If FILE is not specified, a date-stamped file will be dropped in the\n"
           "  current directory.\n" "  See man " SCROT_PACKAGE " for more details\n"
           "  -h, --help                display this help and exit\n"
           "  -v, --version             output version information and exit\n"
           "  -a, --autoselect          non-interactively choose a rectangle of x,y,w,h\n"
           "  -b, --border              When selecting a window, grab wm border too\n"
           "  -c, --count               show a countdown before taking the shot\n"
           "  -d, --delay NUM           wait NUM seconds before taking a shot\n"
           "  -e, --exec APP            run APP on the resulting screenshot\n"
           "  -q, --quality NUM         Image quality (1-100) high value means\n"
           "                            high size, low compression. Default: 75.\n"
           "                            For lossless compression formats, like png,\n"
           "                            low quality means high compression.\n"
           "  -m, --multidisp           For multiple heads, grab shot from each\n"
           "                            and join them together.\n"
           "  -s, --select              interactively choose a window or rectangle\n"
           "                            with the mouse\n"
           "  -u, --focused             use the currently focused window\n"
           "  -t, --thumb NUM           generate thumbnail too. NUM is the percentage\n"
           "                            of the original size for the thumbnail to be,\n"
           "                            or the geometry in percent, e.g. 50x60 or 80x20.\n"
           "  -z, --silent              Prevent beeping\n"
           "  -p, --pointer             Capture the mouse pointer.\n"
           "  -o, --overwrite           By default " SCROT_PACKAGE " does not overwrite the files, use this option to allow it.\n"
           "  -l, --line                Indicates the style of the line when the selection is used: --select\n"
           "                            See SELECTION STYLE\n"
           "  -n, --note                Draw a text note.\n"
           "                            See NOTE FORMAT\n"

           "\n" "  SPECIAL STRINGS\n"
           "  Both the --exec and filename parameters can take format specifiers\n"
           "  that are expanded by " SCROT_PACKAGE " when encountered.\n"
           "  There are two types of format specifier. Characters preceded by a '%%'\n"
           "  are interpreted by strftime(2). See man strftime for examples.\n"
           "  These options may be used to refer to the current date and time.\n"
           "  The second kind are internal to " SCROT_PACKAGE
           "  and are prefixed by '$'\n"
           "  The following specifiers are recognised:\n"
           "                  $a hostname\n"
           "                  $f image path/filename (ignored when used in the filename)\n"
           "                  $m thumbnail path/filename\n"
           "                  $n image name (ignored when used in the filename)\n"
           "                  $s image size (bytes) (ignored when used in the filename)\n"
           "                  $p image pixel size\n"
           "                  $w image width\n"
           "                  $h image height\n"
           "                  $t image format (ignored when used in the filename)\n"
           "                  $$  prints a literal '$'\n"
           "                  \\n prints a newline (ignored when used in the filename)\n"
           "  Example:\n" "          " SCROT_PACKAGE
           " '%%Y-%%m-%%d_$wx$h_scrot.png' -e 'mv $f ~/images/shots/'\n"
           "          Creates a file called something like 2000-10-30_2560x1024_scrot.png\n"
           "          and moves it to your images directory.\n" "\n"
           "\n" "  SELECTION STYLE\n"
           "  When using --select you can indicate the style of the line with --line.\n"
           "  The following specifiers are recognised:\n"
           "                  width=(range 1 to 8),color=\"value\"\n"
           "  For the color you can use a name or a hexdecimal value.\n"
           "                  color=\"red\" or color=\"#ff0000\"\n"
           "  The default style are:\n"
           "                  color=\"gray\",width=1\n"
           "  Example:\n" "          " SCROT_PACKAGE
           " --line width=3,color=\"red\" --select\n\n"
           "\n" "  NOTE FORMAT\n"
           "  The following specifiers are recognised for the option --note\n"
           "                  -f 'FontName/size'\n"
           "                  -t 'text'\n"
           "                  -x position (optional)\n"
           "                  -y position (optional)\n"
           "                  -c color(RGBA) (optional)\n"
           "                  -a angle (optional)\n"
           "  Example:\n" "          " SCROT_PACKAGE
           " --note \"-f '/usr/share/fonts/TTF/DroidSans-Bold/40' -x 10 -y 20 -c 255,0,0,255 -t 'Hi'\"\n\n"
           "This program is free software see the file COPYING for licensing info.\n"
           "Copyright Tom Gilbert 2000\n"
           "Email bugs to <scrot_sucks@linuxbrit.co.uk>\n");
   exit(0);
}
