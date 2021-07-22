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
Copyright 2020      daltomi <daltomi@disroot.org>-
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2020      spycapitan <spycapitan@protonmail.com>

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
#include <assert.h>

#define MAX_LEN_WINDOW_CLASS_NAME  80

static void scrot_parse_option_array(int argc, char **argv);
scrotoptions opt;

void
init_parse_options(int argc, char **argv)
{
   /* Set default options */
   memset(&opt, 0, sizeof(scrotoptions));

   opt.quality = 75;
   opt.overwrite = 0;
   opt.line_style = LineSolid;
   opt.line_width = 1;
   opt.line_opacity = 100;
   opt.line_mode = LINE_MODE_CLASSIC;

   /* Parse the cmdline args */
   scrot_parse_option_array(argc, argv);
}

int
options_parse_required_number(char *str)
{
   char *end = NULL;
   int ret = 0;
   errno = 0;

   if (str != NULL) {
      ret = strtol(str, &end, 10);
   }

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
   enum {STYLE = 0, WIDTH, COLOR, OPACITY, MODE};

   char *const token[] = {
      [STYLE]   = "style",
      [WIDTH]   = "width",
      [COLOR]   = "color",
      [OPACITY] = "opacity",
      [MODE]    = "mode",
      NULL
   };

   char *subopts = optarg;
   char *value = NULL;

   while (*subopts != '\0') {
      switch(getsubopt(&subopts, token, &value)) {

         case STYLE:

            if (value == NULL || *value == '\0') {
               fprintf(stderr, "Missing value for "
                     "suboption '%s'\n", token[STYLE]);
               exit(EXIT_FAILURE);
            }

            if (!strncmp(value, "dash", 4))
               opt.line_style = LineOnOffDash;
            else if (!strncmp(value, "solid", 5))
               opt.line_style = LineSolid;
            else {
               fprintf(stderr, "Unknown value for "
                     "suboption '%s': %s\n", token[STYLE], value);
               exit(EXIT_FAILURE);
            }
            break;

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

         case MODE:

            if (value == NULL || *value == '\0') {
               fprintf(stderr, "Missing value for "
                     "suboption '%s'\n", token[MODE]);
               exit(EXIT_FAILURE);
            }

            bool isValidMode = (bool)(0 == strncmp(value, LINE_MODE_CLASSIC, LINE_MODE_CLASSIC_LEN) );

            isValidMode = isValidMode || (bool)(0 == strncmp(value, LINE_MODE_EDGE, LINE_MODE_EDGE_LEN));

            if(isValidMode == false) {
               fprintf(stderr, "Unknown value for "
                     "suboption '%s': %s\n", token[MODE], value);
               exit(EXIT_FAILURE);
            }

            opt.line_mode = strdup(value);

            break;

         case OPACITY:

            if (value == NULL) {
               fprintf(stderr, "Missing value for "
                    "suboption '%s'\n", token[OPACITY]);
               exit(EXIT_FAILURE);
            }

            opt.line_opacity = options_parse_required_number(value);

            if (opt.line_opacity < 10 || opt.line_opacity > 100){
               fprintf(stderr, "Value of the range (10..100) for "
                     "suboption '%s': %d\n", token[OPACITY], opt.line_opacity);
               exit(EXIT_FAILURE);
             }

            break;

         default:
            fprintf(stderr, "No match found for token: '%s'\n", value);
            exit(EXIT_FAILURE);
            break;
      }

   } /* while */
}

static void options_parse_window_class_name(const char* window_class_name)
{
    assert(window_class_name != NULL);

    if (window_class_name[0] != '\0' &&
        strlen(window_class_name) < MAX_LEN_WINDOW_CLASS_NAME)
    {
        opt.window_class_name = strdup(window_class_name);
    }
}


static void
scrot_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "a:ofpbcd:e:hmq:st:uv+:zn:l:D:kC:S:";

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
      {"freeze", 0, 0, 'f'},
      {"overwrite", 0, 0, 'o'},
      {"stack", 0, 0,'k'},
      /* toggles */
      {"thumb", 1, 0, 't'},
      {"delay", 1, 0, 'd'},
      {"quality", 1, 0, 'q'},
      {"exec", 1, 0, 'e'},
      {"debug-level", 1, 0, '+'},
      {"autoselect", required_argument, 0, 'a'},
      {"display", required_argument, 0, 'D'},
      {"note", required_argument, 0, 'n'},
      {"line", required_argument, 0, 'l'},
      {"class", required_argument, 0, 'C'},
      {"script", required_argument, 0, 'S'},
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
           opt.exec = strdup(optarg);
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
        case 'f':
           opt.freeze = 1;
           break;
        case 'o':
           opt.overwrite = 1;
           break;
        case 'a':
           options_parse_autoselect(optarg);
           break;
        case 'D':
           options_parse_display(optarg);
           break;
        case 'n':
           options_parse_note(optarg);
           break;
        case 'l':
           options_parse_line(optarg);
           break;
        case 'k':
           opt.stack = 1;
        break;
        case 'C':
            options_parse_window_class_name(optarg);
        break;
        case 'S':
           opt.script = strdup(optarg);
        break;
        case '?':
           exit(EXIT_FAILURE);
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
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
         fprintf(stderr, "unrecognised option %s\n", argv[optind++]);
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
   new_title = malloc(length);
   if (! new_title) {
     fprintf(stderr, "Unable to allocate thumbnail: %s", strerror(errno));
     exit(EXIT_FAILURE);
   }

   dot_pos = strrchr(name, '.');
   if (dot_pos)
   {
      diff = (dot_pos - name) / sizeof(char);

      strncpy(new_title, name, diff);
      strcat(new_title, "-thumb");
      strcat(new_title, dot_pos);
   }
   else {
      snprintf(new_title, length, "%s-thumb", name);
   }

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

     if (i != 4)
     {
       fprintf(stderr, "option 'autoselect' require 4 arguments\n");
       exit(EXIT_FAILURE);
     }
   }
   else {
       fprintf(stderr, "invalid format for option -- 'autoselect'\n");
       exit(EXIT_FAILURE);
   }


}

void
options_parse_display(char *optarg)
{
#if SCROT_HAVE_STRNDUP
    opt.display = strndup(optarg, 256);
#else
   size_t length = 0;
   char *new_display;

   length = strlen(optarg) + 1;
   if (length > 256) {
     length = 256;
   }
   new_display = malloc(length);
   if (! new_display) {
     fprintf(stderr, "Unable to allocate display: %s", strerror(errno));
     exit(EXIT_FAILURE);
   }
   strncpy(new_display, optarg, length);
   opt.display=new_display;
#endif
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
   opt.note = strdup(optarg);

   if (opt.note == NULL) return;

   if (opt.note[0] == '\0') {
      fprintf(stderr, "Required arguments for --note.");
      exit(EXIT_FAILURE);
   }

   scrot_note_new(opt.note);
}

/*
Return:
    0 : It does not match
    1 : If it matches
*/
int options_cmp_window_class_name(const char* target_class_name)
    {
    assert(target_class_name != NULL);
    assert(opt.window_class_name != NULL);
    return !!(!strncmp(target_class_name, opt.window_class_name, MAX_LEN_WINDOW_CLASS_NAME - 1));
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
           "  -D, --display             Set DISPLAY target other than current\n"
           "  -a, --autoselect          non-interactively choose a rectangle of x,y,w,h\n"
           "  -b, --border              When selecting a window, grab wm border too\n"
           "                            Use with --select to raise the focus of the window.\n"
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
           "                            with the mouse (use the arrow keys to resize)\n"
           "  -u, --focused             use the currently focused window\n"
           "  -t, --thumb NUM           generate thumbnail too. NUM is the percentage\n"
           "                            of the original size for the thumbnail to be,\n"
           "                            or the geometry in percent, e.g. 50x60 or 80x20.\n"
           "  -z, --silent              Prevent beeping\n"
           "  -p, --pointer             Capture the mouse pointer.\n"
           "  -f, --freeze              Freeze the screen when the selection is used: --select\n"
           "  -o, --overwrite           By default " SCROT_PACKAGE " does not overwrite the files, use this option to allow it.\n"
           "  -l, --line                Indicates the style of the line when the selection is used: --select\n"
           "                            See SELECTION STYLE\n"
           "  -n, --note                Draw a text note.\n"
           "                            See NOTE FORMAT\n"
           "  -k, --stack               Capture stack/overlapped windows and join them together.\n"
           "                            A running Composite Manager is needed.\n"
           "  -C,  --class NAME         Window class name. Associative with options: -k\n"
           "  -S,  --script CMD         Imlib2 script commands\n"
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
           "                  style=(solid,dash),width=(range 1 to 8),color=\"value\",\n"
           "                  opacity=(range 10 to 100),mode=(edge,classic)\n"
           "  The default style is:\n"
           "                  mode=classic,style=solid,width=1,opacity=100\n"
           "  Mode 'edge' ignore    : style, --freeze\n"
           "  Mode 'classic' ignore : opacity\n\n"
           "  The 'opacity' specifier is only effective if a Composite Manager is running.\n\n"
           "  For the color you can use a name or a hexadecimal value.\n"
           "                  color=\"red\" or color=\"#ff0000\"\n"
           "  Example:\n" "          " SCROT_PACKAGE
           " --line style=dash,width=3,color=\"red\" --select\n\n"
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
