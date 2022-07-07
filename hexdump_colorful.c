#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int colorRand = 1;
int seed = 0;

int istrue(int c) { return c; }

char isHighlighted[256];

/*
 * A data structure that holds information about the different character
 * classes, including filter functions, representative symbols and colors, and
 * a brief description for a usage message.
 */
struct characterClass_t {
  char c;
  char *function_name;
  int (*function)(int);
  int active;
  char *description;
  int colorA;
  int colorB;
};

struct characterClass_t characterClass[] = {
    {'A', "isalpha", isalpha, 0, "Checks for an alphabetic character.\n", 1, 29},
    {'P', "isprint", isprint, 0, "Checks for any printable character including space.\n", 1, 30},
    {'a', "isalnum", isalnum, 0, "Checks for an alphanumeric character.\n", 1, 31},
    {'b', "isblank", isblank, 0, "Checks for a blank character; that is, a space or a tab.\n", 1, 32},
    {'d', "isdigit", isdigit, 0, "Checks for a digit (0 through 9).\n", 1, 33},
    {'g', "isgraph", isgraph, 0, "Checks for any printable character except space.\n", 1, 34},
    {'i', "isascii", isascii, 0, "Checks whether it is a 7-bit unsigned char value that fits into the ASCII character set.\n", 1, 35},
    {'l', "islower", islower, 0, "Checks for a lowercase character.\n", 1, 36},
    {'n', "iscntrl", iscntrl, 0, "Checks for a control character.\n", 0, 29},
    {'p', "ispunct", ispunct, 0, "Checks for any printable character which is not a space or an alphanumeric character.\n", 0, 31},
    {'s', "isspace", isspace, 0, "Checks for white-space characters.\n", 0, 32},
    {'u', "isupper", isupper, 0, "Checks for an uppercase letter.\n", 0, 33},
    {'x', "isxdigit", isxdigit, 0, "Checks for hexadecimal digits, that is, one of \"123456789abcdefABCDEF\".\n", 0, 34},
    {'z', "istrue", istrue, 0, "Checks the truth value of the character.\n", 0, 35},
};
int numFuncs = sizeof(characterClass) / sizeof(struct characterClass_t);

/*
 * Outputs color codes according to the provided value, and the rules specified
 * by the user via command line arguments.
 */
void setColor(int c) {
  int colored = 0;

  /*
   * Highlight the area according to the functions in the list of active
   * character classes.
   */
  for (int i = 0; i < numFuncs; i++) {
    if (characterClass[i].active) {
      if (characterClass[i].function(c)) {
        printf("\033[%d;%dm", characterClass[i].colorA, characterClass[i].colorB);
        colored = 1;
      }
    }
  }

  /*
   * Handle the highlighted bytes specified by the `-H` argument.
   */
  for (int i = 0; i < strlen(isHighlighted); i++) {
    if (c == isHighlighted[i]) {
      printf("\033[1;%dm", 31);
      colored = 1;
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
    printf("\033[38;5;%dm", c);
  }
}

/*
 * Clears the current color of the output.
 */
void clearColor() { fprintf(stdout, "\033[0m"); }

/*
 * Print out a usage statement and exit.
 */
void usage(char *argv[]) {
  fprintf(
      stderr,
      "Usage: %s [-aAibndglPpsux] [-C] [-S seed] [-o] [-H character] [-X "
      "character] filename (- for stdin)\n\n"
      " -C\tAllow non-console output.\n"
      " -S\tSpecify the seed to use for random color generation.\n"
      " -o\tTurn off the random color generation.\n"
      " -Y\tDisplay syntax highlighting color key.\n\n"
      " -H character\tHighlight a character. Can be specified multiple times.\n"
      " -X character\tHighlight a character by hex value. Can be specified "
      "multiple times.\n\n"
      "Highlight character classes defined by <ctype.h> by specifying one of "
      "the following switches:\n\n",
      argv[0]);

  for (int i = 0; i < numFuncs; i++) {
    fprintf(stderr, " -%c %s\t%s", characterClass[i].c,
            characterClass[i].function_name,
            characterClass[i].description);
  }
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  int consoleOverride = 0;

  bzero(isHighlighted, 256);

  char classOptions[255];
  bzero(classOptions, 255);
  for (int i = 0; i < numFuncs; i++) {
    classOptions[i] = characterClass[i].c;
  }

  char optstring[255];
  sprintf(optstring, "%sS:oH:X:CY", classOptions);

  /*
   * Process command line arguments
   */
  int opt;
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    int something = 0;
    for (int i = 0; i < numFuncs; i++) {
      if (opt == characterClass[i].c) {
        characterClass[i].active = 1;
        something = 1;
      }
    }
    if (opt == 'H') {
      something = 1;
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
      for (int i = 0; i < numFuncs; i++) {
        printf("[%d;%dm", characterClass[i].colorA, characterClass[i].colorB);
        fprintf(stdout, " -%c %s\t%s", characterClass[i].c,
                characterClass[i].function_name,
                characterClass[i].description);
        clearColor();
      }
      exit(EXIT_SUCCESS);
    }
    if (opt == 'X') {
      something = 1;
      int num = strtol(optarg, NULL, 16);
      isHighlighted[strlen(isHighlighted)] = num;
    }
    if (opt == 'S') {
      something = 1;
      seed = atoi(optarg);
    }
    if (opt == 'o') {
      colorRand = 0;
      something = 1;
    }
    if (opt == 'C') {
      consoleOverride = 1;
      something = 1;
    }
    if (something == 0) {
      usage(argv);
    }
  }

  /*
   * Refuse to output to a file
   */
  if(!consoleOverride && !isatty(fileno(stdout))){
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
  while (1) {

    /*
     * Read in chunks of 16 bytes.
     */
    unsigned char buf[16];
    bzero(buf, 16);
    int len = fread(buf, 1, 16, f);
    if (len == 0) {
      clearColor();
      break;
    }

    /*
     * Print out the hex representation of the byte.
     */
    int i;
    for (i = 0; i < len; i++) {
      unsigned char c = buf[i];
      setColor(c);
      printf("%2.2x ", c);
      clearColor();
    }

    /*
     * Extra padding if less than 16 bytes were read.
     */
    for (; i < 16; i++) {
      printf("   ");
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
      unsigned char c = buf[i];
      if (isalnum(c) || ispunct(c)) {
        setColor(c);
        printf("%c", c);
        clearColor();
      } else {
        setColor(c);
        printf(".");
        clearColor();
      }
    }
    printf("\n");
  }
  exit(EXIT_SUCCESS);
}
