#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>
#include <assert.h>


word_t vaddr_read(vaddr_t addr, int len);

enum {
    TK_NOTYPE,
    TK_OR, TK_AND,                           // or, and         rank 1
    TK_EQ, TK_UNEQ,                          // equal, unequal  rank 2
    /* TODO: Add more token types */
    TK_PLUS, TK_MINUS,                       // +, -            rank 3
    TK_MULTI, TK_DIVIDE, TK_MOD,             // *, /, %         rank 4
    TK_NEG, TK_DEREF,                        // -, *            rank 5
    TK_IMM, TK_REG, TK_VAR,                  // immediate registers variable
    TK_LEFT, TK_RIGHT,                       // left parenthesis, right parenthesis
};


static struct rule {
    const char *regex;
    int token_type;
} rules[] = {

        /* TODO: Add more rules.
         * Pay attention to the precedence level of different rules.
         */

        {" +", TK_NOTYPE},              // spaces

        {"\\|\\|", TK_OR},                 // or
        {"\\&\\&", TK_AND},              // and

        {"==", TK_EQ},                  // equal
        {"!=", TK_UNEQ},                  // unequal

        {"\\+", TK_PLUS},               // plus
        {"\\-", TK_MINUS},               // minus
        {"\\*", TK_MULTI},               // multiply or dereference
        {"\\/", TK_DIVIDE},               // divide
        {"\\%", TK_MOD},                 // mod

        {"\\-", TK_NEG},               // neg -
        {"\\*", TK_DEREF},               // dereference *

        {"(0[xX][0-9a-fA-F]+)|[0-9]+", TK_IMM},  // immediate number
        {"\\$\\w+", TK_REG},
        {"[_A-Za-z]\\w*", TK_VAR},
        {"\\(", TK_LEFT},
        {"\\)", TK_RIGHT},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;



/* judge whether the token of the specified index is a operator */
static bool is_operator(int idx)
{
	    return TK_OR <= tokens[idx].type && 
		tokens[idx].type <= TK_DEREF;
}


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            // i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token token;
        token.type = i;
        memset(token.str, 0, sizeof(token.str));

        switch (rules[i].token_type) {
          case TK_NOTYPE:
		  	break;
          default:
		  	strncpy(token.str, substr_start, substr_len);
			tokens[nr_token++] = token;
			break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // check the operator (dereference and negative)
  for (int i = 0; i < nr_token; i++)
  {
	  if ((tokens[i].type == TK_MULTI || tokens[i].type == TK_MINUS) && (i == 0 || is_operator(i - 1)))
		  tokens[i].type += 4;
  }

  return true;
}


/* judge whether the expression is valid */
static bool is_valid(int p, int q)
{
	int lefts = 0;
	int i;
	for (i = p; i <= q; i++)
	{
		if (tokens[i].type == TK_LEFT)
			lefts++;
		else if (tokens[i].type == TK_RIGHT)
			lefts--;
		if (lefts < 0)
			break;
	}
	return lefts == 0;
}


// Determine the priority of the operator of the specified index
static int operator_rank(int idx)
{
//    The current primary operator has not been found
    if (idx == -1)
        return 6;
    switch (tokens[idx].type) {
        case TK_OR:
        case TK_AND:
            return 1;
        case TK_EQ:
        case TK_UNEQ:
            return 2;
        case TK_PLUS:
        case TK_MINUS:
            return 3;
        case TK_MULTI:
        case TK_DIVIDE:
        case TK_MOD:
            return 4;
        case TK_NEG:
        case TK_DEREF:
            return 5;
        default:
            printf("tokens[%d] is not an operator\n", idx);
            assert(0);
    }
}


// p to q is a complete expression ( expr op expr )
static bool check_parentheses(int p, int q)
{
    if (!(tokens[p].type == TK_LEFT && tokens[q].type == TK_RIGHT))
        return false;
    int lefts = 0;
    int i;
    for (i = p; i <= q; i++)
    {
        if (tokens[i].type == TK_LEFT)
            lefts++;
        else if (tokens[i].type == TK_RIGHT)
            lefts--;
        if (!lefts)
            break;
    }
    return i == q;
}



static word_t eval(int p, int q) {
    if (p > q) {
//        assert(0);
    } else if (p == q) {
        /* Single token.
         * For now this token should be a number.
         * Return the value of the number.
         */
        switch(tokens[p].type)
        {
            case TK_IMM:
        		return strtol(tokens[p].str, NULL, 0);
            case TK_REG:
            {

                bool success;
                word_t val = isa_reg_str2val(tokens[p].str + 1, &success);
                if (success)
                    return val;
                assert(0);
            }

            case TK_VAR:
                return 8;
            default:
                assert(0);
        }

    } else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1);
        // If there is an entire expression to the right of the deref sign
    } else if (tokens[p].type == TK_DEREF && (p + 1 == q || check_parentheses(p + 1, q))) {
        return vaddr_read(eval(p + 1, q), 4);
        // If there is an entire expression to the right of the negative sign
    } else if (tokens[p].type == TK_NEG && (p + 1 == q || check_parentheses(p + 1, q))) {
        return -eval(p + 1, q);
    } else {
        //        op = the position of 主运算符 in the token expression;
        int idx = p;
        int master = -1;
        while (idx <= q)
        {
            //            if the type is left parentheses, then goto the right parentheses
            //              NOTICE: which match it
            if (tokens[idx].type == TK_LEFT)
            {
                int lefts = 0;
                do
                {
                    if (tokens[idx].type == TK_LEFT)
                        lefts++;
                    else if (tokens[idx].type == TK_RIGHT)
                        lefts--;
                }
                while (idx++ < q && lefts != 0);
            }

            else {
                if (is_operator(idx) && operator_rank(idx) <= operator_rank(master))
                    master = idx;
                idx++;
            }
        }


        int val1 = eval(p, master - 1);
        int val2 = eval(master + 1, q);

        switch (tokens[master].type) {
            case TK_OR: return val1 || val2;
            case TK_AND: return val1 && val2;
            case TK_EQ: return val1 == val2;
            case TK_UNEQ: return val1 != val2;
            case TK_PLUS: return val1 + val2;
            case TK_MINUS: return val1 - val2;
            case TK_MULTI: return val1 * val2;
            case TK_DIVIDE: return val1 / val2;
            case TK_MOD: return val1 % val2;
            default:
                assert(0);
        }
    }
    return -1;
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  bool valid = is_valid(0, nr_token - 1);
  if (!valid)
  {
	  Log("invalid expression: %s\n", e);
	  assert(0);
  }

  *success = true;
  return eval(0, nr_token - 1);
}
