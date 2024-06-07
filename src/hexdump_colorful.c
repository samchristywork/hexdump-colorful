#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#include "version.h"

bool colorRand = true;
s32 seed = 0;

s32 istrue(s32 c) { return c; }

char isHighlighted[256];

/*
 * A data structure that holds information about the different character
 * classes, including filter functions, representative symbols and colors, and
 * a brief description for a usage message.
 */
struct characterClass_t {
  char c;
  char *longopt;
  char *function_name;
  s32 (*function)(s32);
  s32 active;
  char *description;
  s32 colorA;
  s32 colorB;
};

struct characterClass_t characterClass[] = {
    {'A', "alpha", "isalpha", isalpha, false, "Checks for an alphabetic character.\n", 1, 29},
    {'P', "print", "isprint", isprint, false, "Checks for any printable character including space.\n", 1, 30},
    {'a', "alnum", "isalnum", isalnum, false, "Checks for an alphanumeric character.\n", 1, 31},
    {'b', "blank", "isblank", isblank, false, "Checks for a blank character; that is, a space or a tab.\n", 1, 32},
    {'d', "digit", "isdigit", isdigit, false, "Checks for a digit (0 through 9).\n", 1, 33},
    {'g', "graph", "isgraph", isgraph, false, "Checks for any printable character except space.\n", 1, 34},
    {'i', "ascii", "isascii", isascii, false, "Checks whether it is a 7-bit unsigned char value that fits into the ASCII character set.\n", 1, 35},
    {'l', "lower", "islower", islower, false, "Checks for a lowercase character.\n", 1, 36},
    {'n', "cntrl", "iscntrl", iscntrl, false, "Checks for a control character.\n", 0, 29},
    {'p', "punct", "ispunct", ispunct, false, "Checks for any printable character which is not a space or an alphanumeric character.\n", 0, 31},
    {'s', "space", "isspace", isspace, false, "Checks for white-space characters.\n", 0, 32},
    {'u', "upper", "isupper", isupper, false, "Checks for an uppercase letter.\n", 0, 33},
    {'x', "xdigit", "isxdigit", isxdigit, false, "Checks for hexadecimal digits, that is, one of \"123456789abcdefABCDEF\".\n", 0, 34},
    {'z', "true", "istrue", istrue, false, "Checks the truth value of the character.\n", 0, 35},
};
s32 numFuncs = sizeof(characterClass) / sizeof(struct characterClass_t);

/*
 * Outputs color codes according to the provided value, and the rules specified
 * by the user via command line arguments.
 */
void setColor(s32 c) {
  s32 colored = false;

  /*
   * Highlight the area according to the functions in the list of active
   * character classes.
   */
  for (s32 i = 0; i < numFuncs; i++) {
    if (characterClass[i].active) {
      if (characterClass[i].function(c)) {
        fprintf(stdout, "\033[%d;%dm", characterClass[i].colorA, characterClass[i].colorB);
        colored = true;
      }
    }
  }

  /*
   * Handle the highlighted bytes specified by the `-H` argument.
   */
  for (s32 i = 0; i < strlen(isHighlighted); i++) {
    if (c == isHighlighted[i]) {
      fprintf(stdout, "\033[1;%dm", 31);
      colored = true;
    }
  }

  /*
   * Add random colors to all of the uncolored output (if colorRand is turned
   * on).
   */
  if (colorRand && !colored) {
    c += seed * 1234;
    c = c % (232 - 17);
    c += 17;
    fprintf(stdout, "\033[38;5;%dm", c);
  }
}

/*
 * Clears the current color of the output.
 */
void clearColor() { fprintf(stdout, "\033[0m"); }

/*
 * Prints the program version
 */
void print_version() {
  printf("%s\n\n%s\n", VERSION_STRING, LICENSE_STRING);
}

/*
 * Print out a usage statement and exit.
 */
void usage(char *argv[]) {
  fprintf(
      stderr,
      "Usage: %s [-aAibndglPpsux] [-C] [-S seed] [-o] [-H character] [-X "
      "character] filename (- for stdin)\n\n"
      " -C,--no-console                Allow non-console output.\n"
      " -S,--seed                      Specify the seed to use for random color generation.\n"
      " -o,--no-random                 Turn off the random color generation.\n"
      " -Y,--key                       Display syntax highlighting color key.\n\n"
      " -v,--version                   Print version information.\n"
      " -H,--highlight     character   Highlight a character. Can be specified multiple times.\n"
      " -X,--hex-highlight character   Highlight a character by hex value. Can be specified multiple times.\n\n"
      "Highlight character classes defined by <ctype.h> by specifying one of "
      "the following switches:\n\n",
      argv[0]);

  for (s32 i = 0; i < numFuncs; i++) {
    fprintf(stderr, " -%c,--%s %s:%s", characterClass[i].c,
            characterClass[i].longopt,
            characterClass[i].function_name,
            characterClass[i].description);
  }
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  bool consoleOverride = false;

  bzero(isHighlighted, 256);

  char classOptions[255];
  bzero(classOptions, 255);
  for (s32 i = 0; i < numFuncs; i++) {
    classOptions[i] = characterClass[i].c;
  }

  /*
   * Process command line arguments
   */
  s32 opt;
  s32 option_index = 0;
  char optstring[512];
  sprintf(optstring, "%sS:oH:X:CYv", classOptions);
  struct option long_options[256] = {
      {"help", no_argument, NULL, 'h'},
      {"no-console", no_argument, NULL, 'C'},
      {"seed", required_argument, NULL, 'S'},
      {"no-random", no_argument, NULL, 'o'},
      {"key", no_argument, NULL, 'Y'},
      {"highlight", required_argument, NULL, 'H'},
      {"hex-highlight", required_argument, NULL, 'X'},
      {"version", no_argument, NULL, 'v'},
  };

  s32 i;
  s32 offset = 7;
  for (i = 0; i < numFuncs; i++) {
    long_options[i + offset].name = characterClass[i].longopt;
    long_options[i + offset].has_arg = no_argument;
    long_options[i + offset].flag = NULL;
    long_options[i + offset].val = characterClass[i].c;
  }
  bzero(&long_options[i + offset + 1], sizeof(struct option));

  while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
    s32 something = false;
    for (s32 i = 0; i < numFuncs; i++) {
      if (opt == characterClass[i].c) {
        characterClass[i].active = 1;
        something = true;
      }
    }
    if (opt == 'H') {
      something = true;
      if (strlen(optarg) != 1) {
        fprintf(stderr, "-H only accepts one character at a time.\n");
        usage(argv);
      }
      if (optarg[0] == 0) {
        fprintf(stderr, "The character specified by -H can't be null. How did "
                        "you even do that?\n");
        usage(argv);
      }
      isHighlighted[strlen(isHighlighted)] = optarg[0];
    }
    if (opt == 'Y') {
      for (s32 i = 0; i < numFuncs; i++) {
        fprintf(stdout, "\033[%d;%dm", characterClass[i].colorA, characterClass[i].colorB);
        fprintf(stdout, " -%c %s\t%s", characterClass[i].c,
                characterClass[i].function_name,
                characterClass[i].description);
        clearColor();
      }
      exit(EXIT_SUCCESS);
    }
    if (opt == 'X') {
      something = true;
      s32 num = strtol(optarg, NULL, 16);
      isHighlighted[strlen(isHighlighted)] = num;
    }
    if (opt == 'v') {
      print_version();
      exit(EXIT_SUCCESS);
    }
    if (opt == 'S') {
      something = true;
      seed = atoi(optarg);
    }
    if (opt == 'o') {
      colorRand = false;
      something = true;
    }
    if (opt == 'C') {
      consoleOverride = true;
      something = false;
    }
    if (something == false) {
      usage(argv);
    }
    if (opt == '?') {
      usage(argv);
    }
  }

  /*
   * Refuse to output to a file
   */
  if (!consoleOverride && !isatty(fileno(stdout))) {
    fprintf(stderr, "This program can only print to a console. -C to override.\n");
    usage(argv);
  }

  if (optind + 1 != argc) {
    fprintf(stderr, "Wrong number of arguments.\n");
    usage(argv);
  }

  /*
   * Open the file specified by the user. "-" is a reserved filename that means
   * "stdin".
   */
  FILE *f;
  if (strncmp(argv[optind], "-", 2) == 0) {
    f = fopen("/dev/stdin", "rb");
  } else {
    f = fopen(argv[optind], "rb");
  }
  if (!f) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  /*
   * Main loop.
   */
  while (true) {

    /*
     * Read in chunks of 16 bytes.
     */
    char buf[16];
    bzero(buf, 16);
    s32 len = fread(buf, 1, 16, f);
    if (len == 0) {
      clearColor();
      break;
    }

    /*
     * Print out the hex representation of the byte.
     */
    s32 i;
    for (i = 0; i < len; i++) {
      char c = buf[i];
      setColor(c);
      fprintf(stdout, "%2.2x ", c);
      clearColor();
    }

    /*
     * Extra padding if less than 16 bytes were read.
     */
    for (; i < 16; i++) {
      fprintf(stdout, "   ");
    }

    /*
     * Divide the different "panes".
     */
    clearColor();
    fprintf(stdout, "| ");

    /*
     * Print out the ASCII representation of the 16 bytes.
     */
    for (i = 0; i < len; i++) {
      char c = buf[i];
      if (isalnum(c) || ispunct(c)) {
        setColor(c);
        fprintf(stdout, "%c", c);
        clearColor();
      } else {
        setColor(c);
        fprintf(stdout, ".");
        clearColor();
      }
    }
    fprintf(stdout, "\n");
  }
  exit(EXIT_SUCCESS);
}
