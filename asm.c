// author: Willian R. Pramio (00262875)
// Assembly Code Generation

#include "asm.h"

// printf("%d", a);
void stringSection(TAC* first)
{
  FILE *fout;
  fout = fopen("out.s", "a");

  // 0 and 1 are reserved
  int i = 2;
  fprintf(fout,
    "\t.section \t.rodata\n"
    ".LC0:\n"
    "\t.string \t\"%%d\\n\"\n"
    ".LC1:\n"
    "\t.string \t\"%%d\"\n"
  );

  for(TAC* tac = first; tac; tac = tac->next)
  {
    if (tac->opcode == TAC_OUTPUT_ARG && tac->res->type == SYMBOL_LIT_STRING)
    {
      fprintf(fout,
      ".LC%d:\n"
      "\t.string \t\"%s\"\n",
      i++, tac->res->text);
    }
  }

  fclose(fout);
}

void globalVariablesSection(TAC* first)
{
  FILE *fout;
  fout = fopen("out.s", "w");

  fprintf(fout,
    "\t.file \t\"input.txt\"\n"
    "\t.text\n");

  int var_c = 0;

  for(TAC* tac = first; tac; tac = tac->next)
  {
    switch (tac->opcode)
    {
    case TAC_VAR_DEC:
      fprintf(fout, "\t; ## TAC_VAR_DEC\n");
      fprintf(fout,
        "\t.globl\t%s\n",
        tac->res->text);
      if (var_c == 0)
        fprintf(fout, "\t.data\n");
      fprintf(fout,
        "\t.align 4\n"
        "\t.type\t%s, @object\n"
        "\t.size\t%s, 4\n"
        "%s:\n"
        "\t.long \t%s\n",
        tac->res->text, tac->res->text,
        tac->res->text, tac->op1 ? tac->op1->text : "0");
      var_c++;
      break;

    case TAC_VEC_DEC:
      fprintf(fout, "\t; ## TAC_VEC_DEC\n");
      if (tac->next->opcode == TAC_VEC_INIT_VALUE)
      {
        fprintf(fout,
        "\t.globl\t%s\n",
        tac->res->text);
        if (var_c == 0)
          fprintf(fout, "\t.data\n");
        fprintf(fout,
          "\t.align 8\n"
          "\t.type\t%s, @object\n"
          "\t.size\t%s, %d\n"
          "%s:\n",
          tac->res->text, tac->res->text,
          4*atoi(tac->op1->text), tac->res->text);
        for(TAC* t = tac->next; t->opcode == TAC_VEC_INIT_VALUE; t = t->next)
          fprintf(fout, "\t.long\t%s\n", t->res->text);
      }
      else
        fprintf(fout,
          "\t.comm	%s,%d,8\n",
        tac->res->text, 4*atoi(tac->op1->text));
      var_c++;
      break;

    case TAC_ARG:
    case TAC_ADD:
    case TAC_SUB:
    case TAC_MULT:
    case TAC_BIGG:
    case TAC_LESS:
    case TAC_LE:
    case TAC_GE:
    case TAC_EQ:
    case TAC_DIF:
    case TAC_AND:
    case TAC_OR:
    case TAC_NEG:
    case TAC_VEC_ACCESS:
    case TAC_INPUT:
      fprintf(fout, "\t; ## TAC_ARG or makeTemp()\n");
      fprintf(fout,
        "\t.globl\t%s\n",
        tac->res->text);
      if (var_c == 0)
        fprintf(fout, "\t.data\n");
      fprintf(fout,
        "\t.align 4\n"
        "\t.type\t%s, @object\n"
        "\t.size\t%s, 4\n"
        "%s:\n"
        "\t.long \t0\n",
        tac->res->text, tac->res->text,
        tac->res->text);
      var_c++;
      break;

    default:
      break;
    }
  }

  fclose(fout);
}

void generateAsm(TAC* first)
{
  globalVariablesSection(first);
  stringSection(first);

  FILE *fout;
  fout = fopen("out.s", "a");

  int func_count = 0;
  int label_count = 0;
  int arg_count = 0;
  // 0 and 1 are reserved
  int string_label_count = 2;
  int i = 0;

  char regs[][10] = {"eax", "edx", "ecx"};
  char arg_regs[][10] = {"edi", "esi", "edx"};

  // main code here
  for(TAC* tac = first; tac; tac = tac->next)
  {
    switch (tac->opcode)
    {
    case TAC_BEGIN_FUNC:
      fprintf(fout, "\t; ## TAC_BEGIN_FUNC\n");
      if (func_count == 0)
        fprintf(fout, "\t.text\n");
      fprintf(fout,
        "\t.globl %s\n"
        "\t.type %s, @function\n"
        "%s:\n"
        ".LFB%d:\n"
        "\t.cfi_startproc\n"
        "\tendbr64\n"
        "\tpushq \t%%rbp\n"
        "\t.cfi_def_cfa_offset 16\n"
        "\t.cfi_offset 6, -16\n"
        "\tmovq \t%%rsp, %%rbp\n"
        "\t.cfi_def_cfa_register 6\n",
      tac->res->text, tac->res->text, tac->res->text, func_count);
      i = 0;
      for(TAC* t = tac->next; t->opcode == TAC_ARG; t = t->next)
      {
        fprintf(fout,
          "\tmovl \t%%%s, %s(%%rip)\n",
          arg_regs[i], t->res->text);
          i++;
      }
      break;

    case TAC_RETURN:
      fprintf(fout, "\t; ## TAC_RETURN\n");
      if (tac->op1->type == SYMBOL_VARIABLE)
        fprintf(fout, "\tmovl \t%s(%%rip), %%eax\n",
          tac->op1->text);
      else
        fprintf(fout, "\tmovl \t$%s, %%eax\n",
          tac->op1->text);
      break;

    case TAC_END_FUNC:
      fprintf(fout, "\t; ## TAC_END_FUNC\n");
      fprintf(fout,
        "\tpopq \t%%rbp\n"
        "\t.cfi_def_cfa 7, 8\n"
        "\tret\n"
        "\t.cfi_endproc\n"
        ".LFE%d:\n"
        "\t.size \t%s, .-%s\n",
        func_count, tac->res->text, tac->res->text
      );
      func_count++;
      break;

    case TAC_COPY:
      fprintf(fout, "\t; ## TAC_COPY\n");
      if (tac->op1->type == SYMBOL_VARIABLE || tac->op1->type == SYMBOL_FUNCTION)
        fprintf(fout,
          "\tmovl \t%%eax, %s(%%rip)\n",
          tac->res->text);
      else
        fprintf(fout,
          "\tmovl \t$%s, %s(%%rip)\n",
          tac->op1->text, tac->res->text);
      break;

    case TAC_COPY_VEC:
      fprintf(fout, "\t; ## TAC_COPY_VEC\n");
      if (tac->op2->type == SYMBOL_VARIABLE)
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tmovl \t%%eax, %d+%s(%%rip)\n",
        tac->op2->text, 4*atoi(tac->op1->text), tac->res->text);
      else
        fprintf(fout,
          "\tmovl \t$%s, %d+%s(%%rip)\n",
        tac->op2->text, 4*atoi(tac->op1->text), tac->res->text);
      break;

    case TAC_ADD:
      fprintf(fout, "\t; ## TAC_ADD\n");
      if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t$%s, %%eax\n"
          "\taddl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\taddl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\taddl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\taddl \t%s(%%rip), %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      // if (tac->next->opcode == TAC_COPY || tac->next->opcode == TAC_COPY_VEC)
      fprintf(fout,
          "\tmovl \t%%eax, %s(%%rip)\n",
        tac->res->text);
      break;

    case TAC_SUB:
      fprintf(fout, "\t; ## TAC_SUB\n");
      if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t$%s, %%eax\n"
          "\tsubl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        // b = a - 987;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tsubl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        // b = 987 - a;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tmovl \t$%s, %%edx\n"
          "\tsubl \t%%eax, %%edx\n"
          "\tmovl \t%%edx, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        // b = a - b;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tsubl \t%%eax, %%edx\n"
          "\tmovl \t%%edx, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout,
        "\tmovl \t%%eax, %s(%%rip)\n",
      tac->res->text);
      break;

    case TAC_MULT:
      fprintf(fout, "\t; ## TAC_MULT\n");
      if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t$%s, %%eax\n"
          "\timull \t$%s, %%eax, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        // b = a * 987;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\timull \t$%s, %%eax, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        // b = 987 * a;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\timull \t$%s, %%eax, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        // b = a * b;
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\timull \t%%edx, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      break;

    case TAC_BIGG:
      fprintf(fout, "\t; ## TAC_BIGG\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tjle \t.%s\n", tac->next->res->text);
      break;

    case TAC_LESS:
      fprintf(fout, "\t; ## TAC_LESS\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tjge \t.%s\n", tac->next->res->text);
      break;

    case TAC_LE:
      fprintf(fout, "\t; ## TAC_LE\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tjg \t.%s\n", tac->next->res->text);
      break;

    case TAC_GE:
      fprintf(fout, "\t; ## TAC_GE\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tjl \t.%s\n", tac->next->res->text);
      break;

    case TAC_EQ:
      fprintf(fout, "\t; ## TAC_EQ\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tjne \t.%s\n", tac->next->res->text);
      break;

    case TAC_DIF:
      fprintf(fout, "\t; ## TAC_DIF\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op1->text, tac->op2->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t$%s, %%eax\n",
        tac->op2->text, tac->op1->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n"
          "\tmovl \t%s(%%rip), %%eax\n"
          "\tcmpl \t%%eax, %%edx\n",
        tac->op1->text, tac->op2->text);
      }
      fprintf(fout, "\tje \t.%s\n", tac->next->res->text);
      break;

    case TAC_AND:
      fprintf(fout, "\t; ## TAC_AND\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\ttestl \t$%s, $%s\n"
          "\tje \t.%s\n",
        tac->op2->text, tac->op2->text, tac->next->res->text);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op1->text, tac->next->res->text);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\ttestl \t$%s, $%s\n"
          "\tje \t.%s\n",
        tac->op1->text, tac->op1->text, tac->next->res->text);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op2->text, tac->next->res->text);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%edx\n"
          "\tje \t.%s\n",
        tac->op1->text, tac->next->res->text);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op2->text, tac->next->res->text);
      }
      break;

    case TAC_OR:
      fprintf(fout, "\t; ## TAC_OR\n");
      if (tac->op1->type == SYMBOL_VARIABLE && tac->op2->type != SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\ttestl \t$%s, $%s\n"
          "\tjne \t.L%d\n",
        tac->op2->text, tac->op2->text, label_count);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op1->text, tac->next->res->text);
        fprintf(fout, ".L%d:\n", label_count++);
      }
      else if (tac->op1->type != SYMBOL_VARIABLE && tac->op2->type == SYMBOL_VARIABLE)
      {
        fprintf(fout,
          "\ttestl \t$%s, $%s\n"
          "\tjne \t.L%d\n",
        tac->op1->text, tac->op1->text, label_count);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op2->text, tac->next->res->text);
        fprintf(fout, ".L%d:\n", label_count++);
      }
      else
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tjne \t.L%d\n",
        tac->op1->text, label_count);
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tje \t.%s\n",
        tac->op2->text, tac->next->res->text);
        fprintf(fout, ".L%d:\n", label_count++);
      }
      break;

    case TAC_NEG:
      fprintf(fout, "\t; ## TAC_NEG\n");
      if (tac->op1->type == SYMBOL_VARIABLE)
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%eax\n"
          "\ttestl \t%%eax, %%eax\n"
          "\tjne \t.%s\n",
        tac->op1->text, tac->next->res->text);
      else
        fprintf(fout,
          "\ttestl \t$%s, $%s\n"
          "\tjne \t.%s\n",
        tac->op1->text, tac->op1->text, tac->next->res->text);
      break;

    case TAC_JUMP:
      fprintf(fout, "\t; ## TAC_JUMP\n");
      fprintf(fout, "\tjmp \t.%s\n", tac->res->text);
      break;

    case TAC_LABEL:
      fprintf(fout, "\t; ## TAC_LABEL\n");
      fprintf(fout, ".%s:\n", tac->res->text);
      break;

    case TAC_VEC_ACCESS:
      fprintf(fout, "\t; ## TAC_VEC_ACCESS\n");
      fprintf(fout,
        "\tmovl \t%d+%s(%%rip), %%eax\n"
        "\tmovl \t%%eax, %s(%%rip)\n",
        4*atoi(tac->op2->text), tac->op1->text,
        tac->res->text);
      break;

    case TAC_VAR_ACCESS:
      fprintf(fout, "\t; ## TAC_VAR_ACCESS\n");
      if (tac->next && tac->next->opcode == TAC_COPY)
        fprintf(fout, "\tmovl \t%s(%%rip), %%eax\n",
          tac->res->text);
      break;

    case TAC_INPUT:
      fprintf(fout, "\t; ## TAC_INPUT\n");
      fprintf(fout,
        "\tleaq \t%s(%%rip), %%rsi\n"
        "\tleaq \t.LC1(%%rip), %%rdi\n"
        "\tmovl \t$0, %%eax\n"
        "\tcall \t__isoc99_scanf@PLT\n"
        "\tmovl \t%s(%%rip), %%eax\n",
        tac->res->text, tac->res->text);
      break;

    case TAC_OUTPUT:
      fprintf(fout, "\t; ## TAC_OUTPUT\n");
      if (tac->next->opcode == TAC_OUTPUT_ARG)
      {
        TAC* arg = tac->next;
        if (arg->res->type == SYMBOL_VECTOR)
          fprintf(fout, "\tmovl \t%d+%s(%%rip), %%eax\n",
          4*atoi(arg->op1->text), arg->res->text);
        else if (arg->res->type == SYMBOL_VARIABLE)
          fprintf(fout, "\tmovl \t%s(%%rip), %%eax\n",
            arg->res->text);
        else if (arg->res->type == SYMBOL_LIT_INT || arg->res->type == SYMBOL_LIT_CHAR)
          fprintf(fout, "\tmovl \t$%s, %%eax\n",
            arg->res->text);
      }
      fprintf(fout,
        "\tmovl \t%%eax, %%esi\n"
        "\tleaq \t.LC%d(%%rip), %%rdi\n"
        "\tmovl \t$0, %%eax\n"
        "\tcall \tprintf@PLT\n",
        tac->next->res->type == SYMBOL_LIT_STRING ? string_label_count++ : 0);
      break;

    case TAC_FUNC_CALL:
      fprintf(fout, "\t; ## TAC_FUNC_CALL\n");
      i = 0;
      for(TAC* t = tac->next; t->opcode == TAC_FUNC_CALL_ARG; t = t->next)
        i++;
      if (i > 0)
        fprintf(fout, "\tmovl \t%s(%%rip), %%eax\n",
          tac->next->res->text);
      if (i == 2)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%edx\n",
          tac->next->next->res->text);
      }
      if (i == 3)
      {
        fprintf(fout,
          "\tmovl \t%s(%%rip), %%ecx\n"
          "\tmovl \t%s(%%rip), %%edx\n",
          tac->next->next->res->text,
          tac->next->next->next->res->text);
      }
      if (i > 0)
        fprintf(fout, "\tmovl \t%%%s, %%esi\n", regs[i-1]);
      fprintf(fout,
        "\tmovl \t%%eax, %%edi\n"
        "\tcall \t%s\n",
        tac->res->text);
      break;

    default:
      break;
    }
  }

  fclose(fout);
}
