#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  {"si", "Let the program execute N instructions in a single step and pause \
   the execution When n is not given, it defaults to 1", cmd_si},
  {"info", "Print register status or monitor point information", cmd_info},
  {"x", "Display the memory content of the specified address in hexadecimal format", cmd_x},
  {"p", "Displays the value of the specified expression", cmd_p},
  {"w", "Pauses the execution of the program when the value of the \
  specified expression changes", cmd_w},
  {"d", "Delete the monitoring point of the specified sequence number", cmd_d},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args)
{
  uint64_t n = 1;
  if (args)
  {
    n = atoi(args);
  }

  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");

  if (arg == NULL)
  {
    printf("info: missing operand\n");
    printf("Try \"help info\" for more information\n");
  }
  else
  {
    /* Print register status */
    if (!strcmp(arg, "r"))
    {
      isa_reg_display();
    }
    /* Print monitor point information */
    else if (!strcmp(arg, "w"))
    {
      show_watchpoints();
    }

    else
    {
      printf("Usage: info [wr]\n");
    }
  }

  return 0;

}

static int cmd_x(char *args)
{
  char * arg1 = strtok(NULL, " ");
  char * arg2 = strtok(NULL, " ");
  int n = atoi(arg1);
  bool success;
  vaddr_t addr = expr(arg2, &success);
  printf("show 0x%08x: \n", addr);

  if (!success)
  {
    Log("expression %s error\n", arg2);
    return 1;
  }

  word_t data;
  int idx = 0;
  int line;
  while(idx < n)
  {
    printf("0x%08x: ", addr);
    line = 4;
    while (idx < n && line--)
    {
      data = vaddr_read(addr, 4);
      printf("0x%08x\t", data);
      idx++;
      addr += 4;
    }
    printf("\n");
  }


  return 0;
}

static int cmd_p(char *args)
{
  static int num = 1;
  char * str_expr = strtok(NULL, " ");
  bool success;
  word_t res = expr(str_expr, &success);
  if (!success)
  {
    Log("expression %s error\n", str_expr);
    return 1;
  }
  printf("$%d = 0x%08x\t%u\n", num++, res, res);

  return 0;
}

static int cmd_w(char *args)
{
  char * str_expr = strtok(NULL, " ");
  return new_wp(str_expr);
}

static int cmd_d(char *args)
{
  int NO = atoi(strtok(NULL, " "));
  return free_wp(NO);
}


void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    /*
     *  Remove leading spaces
     */
    while (isblank(*args))
        args++;

    if (args >= str_end) {
      args = NULL;
    }
    
    

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
