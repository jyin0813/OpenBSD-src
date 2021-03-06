/*	$OpenBSD: syn.c,v 1.8 1997/09/01 18:30:12 deraadt Exp $	*/

/*
 * shell parser (C version)
 */

#include "sh.h"
#include "c_test.h"

struct multiline_state {
	int	on;		/* set in multiline commands (\n becomes ;) */
	int	start_token;	/* token multiline is for (eg, FOR, {, etc.) */
	int	start_line;	/* line multiline command started on */
};

static void	yyparse		ARGS((void));
static struct op *pipeline	ARGS((int cf));
static struct op *andor		ARGS((void));
static struct op *c_list	ARGS((void));
static struct ioword *synio	ARGS((int cf));
static void	musthave	ARGS((int c, int cf));
static struct op *nested	ARGS((int type, int smark, int emark));
static struct op *get_command	ARGS((int cf));
static struct op *dogroup	ARGS((void));
static struct op *thenpart	ARGS((void));
static struct op *elsepart	ARGS((void));
static struct op *caselist	ARGS((void));
static struct op *casepart	ARGS((int endtok));
static struct op *function_body	ARGS((char *name, int ksh_func));
static char **	wordlist	ARGS((void));
static struct op *block		ARGS((int type, struct op *t1, struct op *t2,
				      char **wp));
static struct op *newtp		ARGS((int type));
static void	syntaxerr	ARGS((const char *what))
						GCC_FUNC_ATTR(noreturn);
static void	multiline_push ARGS((struct multiline_state *save, int tok));
static void	multiline_pop ARGS((struct multiline_state *saved));
static int	assign_command ARGS((char *s));
static int	inalias ARGS((struct source *s));
#ifdef KSH
static int	dbtestp_isa ARGS((Test_env *te, Test_meta meta));
static const char *dbtestp_getopnd ARGS((Test_env *te, Test_op op,
					int do_eval));
static int	dbtestp_eval ARGS((Test_env *te, Test_op op, const char *opnd1,
				const char *opnd2, int do_eval));
static void	dbtestp_error ARGS((Test_env *te, int offset, const char *msg));
#endif /* KSH */

static	struct	op	*outtree; /* yyparse output */

static struct multiline_state multiline;	/* \n changed to ; */

static	int	reject;		/* token(cf) gets symbol again */
static	int	symbol;		/* yylex value */

#define	REJECT	(reject = 1)
#define	ACCEPT	(reject = 0)
#define	token(cf) \
	((reject) ? (ACCEPT, symbol) : (symbol = yylex(cf)))
#define	tpeek(cf) \
	((reject) ? (symbol) : (REJECT, symbol = yylex(cf)))

static void
yyparse()
{
	int c;

	ACCEPT;
	yynerrs = 0;

	outtree = c_list();
	c = tpeek(0);
	if (c == 0 && !outtree)
		outtree = newtp(TEOF);
	else if (c != '\n' && c != 0)
		syntaxerr((char *) 0);
}

static struct op *
pipeline(cf)
	int cf;
{
	register struct op *t, *p, *tl = NULL;

	t = get_command(cf);
	if (t != NULL) {
		while (token(0) == '|') {
			if ((p = get_command(CONTIN)) == NULL)
				syntaxerr((char *) 0);
			if (tl == NULL)
				t = tl = block(TPIPE, t, p, NOWORDS);
			else
				tl = tl->right = block(TPIPE, tl->right, p, NOWORDS);
		}
		REJECT;
	}
	return (t);
}

static struct op *
andor()
{
	register struct op *t, *p;
	register int c;

	t = pipeline(0);
	if (t != NULL) {
		while ((c = token(0)) == LOGAND || c == LOGOR) {
			if ((p = pipeline(CONTIN)) == NULL)
				syntaxerr((char *) 0);
			t = block(c == LOGAND? TAND: TOR, t, p, NOWORDS);
		}
		REJECT;
	}
	return (t);
}

static struct op *
c_list()
{
	register struct op *t, *p, *tl = NULL;
	register int c;

	t = andor();
	if (t != NULL) {
		/* Token has always been read/rejected at this point, so
		 * we don't worray about what flags to pass token()
		 */
		while ((c = token(0)) == ';' || c == '&' || c == COPROC ||
		       (c == '\n' && (multiline.on || inalias(source))))
		{
			if (c == '&' || c == COPROC) {
				int type = c == '&' ? TASYNC : TCOPROC;
				if (tl)
					tl->right = block(type, tl->right,
							  NOBLOCK, NOWORDS);
				else
					t = block(type, t, NOBLOCK, NOWORDS);
			}
			if ((p = andor()) == NULL)
				return (t);
			if (tl == NULL)
				t = tl = block(TLIST, t, p, NOWORDS);
			else
				tl = tl->right = block(TLIST, tl->right, p, NOWORDS);
		}
		REJECT;
	}
	return (t);
}

static struct ioword *
synio(cf)
	int cf;
{
	register struct ioword *iop;
	int ishere;

	if (tpeek(cf) != REDIR)
		return NULL;
	ACCEPT;
	iop = yylval.iop;
	ishere = (iop->flag&IOTYPE) == IOHERE;
	musthave(LWORD, ishere ? HEREDELIM : 0);
	if (ishere) {
		iop->delim = yylval.cp;
		if (*ident != 0) /* unquoted */
			iop->flag |= IOEVAL;
		if (herep >= &heres[HERES])
			yyerror("too many <<'s\n");
		*herep++ = iop;
	} else
		iop->name = yylval.cp;
	return iop;
}

static void
musthave(c, cf)
	int c, cf;
{
	if ((token(cf)) != c)
		syntaxerr((char *) 0);
}

static struct op *
nested(type, smark, emark)
	int type, smark, emark;
{
	register struct op *t;
	struct multiline_state old_multiline;

	multiline_push(&old_multiline, smark);
	t = c_list();
	musthave(emark, KEYWORD|ALIAS);
	multiline_pop(&old_multiline);
	return (block(type, t, NOBLOCK, NOWORDS));
}

static struct op *
get_command(cf)
	int cf;
{
	register struct op *t;
	register int c, iopn = 0, syniocf;
	struct ioword *iop, **iops;
	XPtrV args, vars;
	struct multiline_state old_multiline;

	iops = (struct ioword **) alloc(sizeofN(struct ioword *, NUFILE+1),
					ATEMP);
	XPinit(args, 16);
	XPinit(vars, 16);

	/* Don't want to pass CONTIN if reading interactively as just hitting
	 * return would print PS2 instead of PS1.
	 */
	if (multiline.on || inalias(source))
		cf = CONTIN;
	syniocf = KEYWORD|ALIAS;
	switch (c = token(cf|KEYWORD|ALIAS|VARASN)) {
	  default:
		REJECT;
		afree((void*) iops, ATEMP);
		XPfree(args);
		XPfree(vars);
		return NULL; /* empty line */

	  case LWORD:
	  case REDIR:
		REJECT;
		syniocf &= ~(KEYWORD|ALIAS);
		t = newtp(TCOM);
		while (1) {
			cf = (t->u.evalflags ? ARRAYVAR : 0)
			     | (XPsize(args) == 0 ? ALIAS|VARASN : CMDWORD);
			switch (tpeek(cf)) {
			  case REDIR:
				if (iopn >= NUFILE)
					yyerror("too many redirections\n");
				iops[iopn++] = synio(cf);
				break;

			  case LWORD:
				ACCEPT;
				/* the iopn == 0 and XPsize(vars) == 0 are
				 * dubious but at&t ksh acts this way
				 */
				if (iopn == 0 && XPsize(vars) == 0
				    && XPsize(args) == 0
				    && assign_command(ident))
					t->u.evalflags = DOVACHECK;
				if ((XPsize(args) == 0 || Flag(FKEYWORD))
				    && is_wdvarassign(yylval.cp))
					XPput(vars, yylval.cp);
				else
					XPput(args, yylval.cp);
				break;

			  case '(':
				/* Check for "> foo (echo hi)", which at&t ksh
				 * allows (not POSIX, but not disallowed)
				 */
				afree(t, ATEMP);
				if (XPsize(args) == 0 && XPsize(vars) == 0) {
					ACCEPT;
					goto Subshell;
				}
				/* Must be a function */
				if (iopn != 0 || XPsize(args) != 1
				    || XPsize(vars) != 0)
					syntaxerr((char *) 0);
				ACCEPT;
				/*(*/
				musthave(')', 0);
				t = function_body(XPptrv(args)[0], FALSE);
				goto Leave;

			  default:
				goto Leave;
			}
		}
	  Leave:
		break;

	  Subshell:
	  case '(':
		t = nested(TPAREN, '(', ')');
		break;

	  case '{': /*}*/
		t = nested(TBRACE, '{', '}');
		break;

#ifdef KSH
	  case MDPAREN:
	  {
		static const char let_cmd[] = { CHAR, 'l', CHAR, 'e',
						CHAR, 't', EOS };
		syniocf &= ~(KEYWORD|ALIAS);
		t = newtp(TCOM);
		ACCEPT;
		XPput(args, wdcopy(let_cmd, ATEMP));
		musthave(LWORD,LETEXPR);
		XPput(args, yylval.cp);
		break;
	  }
#endif /* KSH */

#ifdef KSH
	  case DBRACKET: /* [[ .. ]] */
		syniocf &= ~(KEYWORD|ALIAS);
		t = newtp(TDBRACKET);
		ACCEPT;
		{
			Test_env te;

			te.flags = TEF_DBRACKET;
			te.pos.av = &args;
			te.isa = dbtestp_isa;
			te.getopnd = dbtestp_getopnd;
			te.eval = dbtestp_eval;
			te.error = dbtestp_error;

			test_parse(&te);
		}
		break;
#endif /* KSH */

	  case FOR:
	  case SELECT:
		t = newtp((c == FOR) ? TFOR : TSELECT);
		musthave(LWORD, ARRAYVAR);
		if (!is_wdvarname(yylval.cp, TRUE))
			yyerror("%s: bad identifier\n",
				c == FOR ? "for" : "select");
		t->str = str_save(ident, ATEMP);
		multiline_push(&old_multiline, c);
		t->vars = wordlist();
		t->left = dogroup();
		multiline_pop(&old_multiline);
		break;

	  case WHILE:
	  case UNTIL:
		multiline_push(&old_multiline, c);
		t = newtp((c == WHILE) ? TWHILE : TUNTIL);
		t->left = c_list();
		t->right = dogroup();
		multiline_pop(&old_multiline);
		break;

	  case CASE:
		t = newtp(TCASE);
		musthave(LWORD, 0);
		t->str = yylval.cp;
		multiline_push(&old_multiline, c);
		t->left = caselist();
		multiline_pop(&old_multiline);
		break;

	  case IF:
		multiline_push(&old_multiline, c);
		t = newtp(TIF);
		t->left = c_list();
		t->right = thenpart();
		musthave(FI, KEYWORD|ALIAS);
		multiline_pop(&old_multiline);
		break;

	  case BANG:
		syniocf &= ~(KEYWORD|ALIAS);
		t = pipeline(0);
		if (t == (struct op *) 0)
			syntaxerr((char *) 0);
		t = block(TBANG, NOBLOCK, t, NOWORDS);
		break;

	  case TIME:
		syniocf &= ~(KEYWORD|ALIAS);
		t = pipeline(0);
		t = block(TTIME, t, NOBLOCK, NOWORDS);
		break;

	  case FUNCTION:
		musthave(LWORD, 0);
		t = function_body(yylval.cp, TRUE);
		break;
	}

	while ((iop = synio(syniocf)) != NULL) {
		if (iopn >= NUFILE)
			yyerror("too many redirections\n");
		iops[iopn++] = iop;
	}

	if (iopn == 0) {
		afree((void*) iops, ATEMP);
		t->ioact = NULL;
	} else {
		iops[iopn++] = NULL;
		iops = (struct ioword **) aresize((void*) iops,
					sizeofN(struct ioword *, iopn), ATEMP);
		t->ioact = iops;
	}

	if (t->type == TCOM || t->type == TDBRACKET) {
		XPput(args, NULL);
		t->args = (char **) XPclose(args);
		XPput(vars, NULL);
		t->vars = (char **) XPclose(vars);
	} else {
		XPfree(args);
		XPfree(vars);
	}

	return t;
}

static struct op *
dogroup()
{
	register int c;
	register struct op *list;

	c = token(CONTIN|KEYWORD|ALIAS);
	/* A {...} can be used instead of do...done for for/select loops
	 * but not for while/until loops - we don't need to check if it
	 * is a while loop because it would have been parsed as part of
	 * the conditional command list...
	 */
	if (c == DO)
		c = DONE;
	else if (c == '{')
		c = '}';
	else
		syntaxerr((char *) 0);
	list = c_list();
	musthave(c, KEYWORD|ALIAS);
	return list;
}

static struct op *
thenpart()
{
	register struct op *t;

	musthave(THEN, KEYWORD|ALIAS);
	t = newtp(0);
	t->left = c_list();
	if (t->left == NULL)
		syntaxerr((char *) 0);
	t->right = elsepart();
	return (t);
}

static struct op *
elsepart()
{
	register struct op *t;

	switch (token(KEYWORD|ALIAS|VARASN)) {
	  case ELSE:
		if ((t = c_list()) == NULL)
			syntaxerr((char *) 0);
		return (t);

	  case ELIF:
		t = newtp(TELIF);
		t->left = c_list();
		t->right = thenpart();
		return (t);

	  default:
		REJECT;
	}
	return NULL;
}

static struct op *
caselist()
{
	register struct op *t, *tl;
	int c;

	c = token(CONTIN|KEYWORD|ALIAS);
	/* A {...} can be used instead of in...esac for case statements */
	if (c == IN)
		c = ESAC;
	else if (c == '{')
		c = '}';
	else
		syntaxerr((char *) 0);
	t = tl = NULL;
	while ((tpeek(CONTIN|KEYWORD|ESACONLY)) != c) { /* no ALIAS here */
		struct op *tc = casepart(c);
		if (tl == NULL)
			t = tl = tc, tl->right = NULL;
		else
			tl->right = tc, tl = tc;
	}
	musthave(c, KEYWORD|ALIAS);
	return (t);
}

static struct op *
casepart(endtok)
	int endtok;
{
	register struct op *t;
	register int c;
	XPtrV ptns;

	XPinit(ptns, 16);
	t = newtp(TPAT);
	c = token(CONTIN|KEYWORD); /* no ALIAS here */
	if (c != '(')
		REJECT;
	do {
		musthave(LWORD, 0);
		XPput(ptns, yylval.cp);
	} while ((c = token(0)) == '|');
	REJECT;
	XPput(ptns, NULL);
	t->vars = (char **) XPclose(ptns);
	musthave(')', 0);

	t->left = c_list();
	if ((tpeek(CONTIN|KEYWORD|ALIAS)) != endtok)
		musthave(BREAK, CONTIN|KEYWORD|ALIAS);
	return (t);
}

static struct op *
function_body(name, ksh_func)
	char *name;
	int ksh_func;	/* function foo { ... } vs foo() { .. } */
{
	XString xs;
	char *xp, *p;
	struct op *t;
	int old_func_parse;

	Xinit(xs, xp, 16, ATEMP);
	for (p = name; ; ) {
		if ((*p == EOS && Xlength(xs, xp) == 0)
		    || (*p != EOS && *p != CHAR && *p != QCHAR
			&& *p != OQUOTE && *p != CQUOTE))
		{
			p = snptreef((char *) 0, 32, "%S", name);
			yyerror("%s: invalid function name\n", p);
		}
		Xcheck(xs, xp);
		if (*p == EOS) {
			Xput(xs, xp, '\0');
			break;
		} else if (*p == CHAR || *p == QCHAR) {
			Xput(xs, xp, p[1]);
			p += 2;
		} else
			p++;	/* OQUOTE/CQUOTE */
	}
	t = newtp(TFUNCT);
	t->str = Xclose(xs, xp);
	t->u.ksh_func = ksh_func;

	/* Note that POSIX allows only compound statements after foo(), sh and
	 * at&t ksh allow any command, go with the later since it shouldn't
	 * break anything.  However, for function foo, at&t ksh only accepts
	 * an open-brace.
	 */
	if (ksh_func) {
		musthave('{', CONTIN|KEYWORD|ALIAS); /* } */
		REJECT;
	}

	old_func_parse = e->flags & EF_FUNC_PARSE;
	e->flags |= EF_FUNC_PARSE;
	if ((t->left = get_command(CONTIN)) == (struct op *) 0) {
		/* create empty command so foo(): will work */
		t->left = newtp(TCOM);
		t->args = (char **) alloc(sizeof(char *), ATEMP);
		t->args[0] = (char *) 0;
		t->vars = (char **) alloc(sizeof(char *), ATEMP);
		t->vars[0] = (char *) 0;
	}
	if (!old_func_parse)
		e->flags &= ~EF_FUNC_PARSE;

	return t;
}

static char **
wordlist()
{
	register int c;
	XPtrV args;

	XPinit(args, 16);
	if ((c = token(CONTIN|KEYWORD|ALIAS)) != IN) {
		if (c != ';') /* non-POSIX, but at&t ksh accepts a ; here */
			REJECT;
		return NULL;
	}
	while ((c = token(0)) == LWORD)
		XPput(args, yylval.cp);
	if (c != '\n' && c != ';')
		syntaxerr((char *) 0);
	if (XPsize(args) == 0) {
		XPfree(args);
		return NULL;
	} else {
		XPput(args, NULL);
		return (char **) XPclose(args);
	}
}

/*
 * supporting functions
 */

static struct op *
block(type, t1, t2, wp)
	int type;
	struct op *t1, *t2;
	char **wp;
{
	register struct op *t;

	t = newtp(type);
	t->left = t1;
	t->right = t2;
	t->vars = wp;
	return (t);
}

const	struct tokeninfo {
	const char *name;
	short	val;
	short	reserved;
} tokentab[] = {
	/* Reserved words */
	{ "if",		IF,	TRUE },
	{ "then",	THEN,	TRUE },
	{ "else",	ELSE,	TRUE },
	{ "elif",	ELIF,	TRUE },
	{ "fi",		FI,	TRUE },
	{ "case",	CASE,	TRUE },
	{ "esac",	ESAC,	TRUE },
	{ "for",	FOR,	TRUE },
#ifdef KSH
	{ "select",	SELECT,	TRUE },
#endif /* KSH */
	{ "while",	WHILE,	TRUE },
	{ "until",	UNTIL,	TRUE },
	{ "do",		DO,	TRUE },
	{ "done",	DONE,	TRUE },
	{ "in",		IN,	TRUE },
	{ "function",	FUNCTION, TRUE },
	{ "time",	TIME,	TRUE },
	{ "{",		'{',	TRUE },
	{ "}",		'}',	TRUE },
	{ "!",		BANG,	TRUE },
#ifdef KSH
	{ "[[",		DBRACKET, TRUE },
#endif /* KSH */
	/* Lexical tokens (0[EOF], LWORD and REDIR handled specially) */
	{ "&&",		LOGAND,	FALSE },
	{ "||",		LOGOR,	FALSE },
	{ ";;",		BREAK,	FALSE },
#ifdef KSH
	{ "((",		MDPAREN, FALSE },
	{ "|&",		COPROC,	FALSE },
#endif /* KSH */
	/* and some special cases... */
	{ "newline",	'\n',	FALSE },
	{ 0 }
};

void
initkeywords()
{
	register struct tokeninfo const *tt;
	register struct tbl *p;

	tinit(&keywords, APERM, 32); /* must be 2^n (currently 20 keywords) */
	for (tt = tokentab; tt->name; tt++) {
		if (tt->reserved) {
			p = tenter(&keywords, tt->name, hash(tt->name));
			p->flag |= DEFINED|ISSET;
			p->type = CKEYWD;
			p->val.i = tt->val;
		}
	}
}

static void
syntaxerr(what)
	const char *what;
{
	char redir[6];	/* 2<<- is the longest redirection, I think */
	const char *s;
	struct tokeninfo const *tt;
	int c;

	if (!what)
		what = "unexpected";
	REJECT;
	c = token(0);
    Again:
	switch (c) {
	case 0:
		if (multiline.on && multiline.start_token) {
			multiline.on = FALSE; /* avoid infinate loops */
			c = multiline.start_token;
			source->errline = multiline.start_line;
			what = "unmatched";
			goto Again;
		}
		/* don't quote the EOF */
		yyerror("syntax error: unexpected EOF\n");
		/*NOTREACHED*/

	case LWORD:
		s = snptreef((char *) 0, 32, "%S", yylval.cp);
		break;

	case REDIR:
		s = snptreef(redir, sizeof(redir), "%R", yylval.iop);
		break;

	default:
		for (tt = tokentab; tt->name; tt++)
			if (tt->val == c)
			    break;
		if (tt->name)
			s = tt->name;
		else {
			if (c > 0 && c < 256) {
				redir[0] = c;
				redir[1] = '\0';
			} else
				shf_snprintf(redir, sizeof(redir),
					"?%d", c);
			s = redir;
		}
	}
	yyerror("syntax error: `%s' %s\n", s, what);
}

static void
multiline_push(save, tok)
	struct multiline_state *save;
	int tok;
{
	*save = multiline;
	multiline.on = TRUE;
	multiline.start_token = tok;
	multiline.start_line = source->line;
}

static void
multiline_pop(saved)
	struct multiline_state *saved;
{
	multiline = *saved;
}

static struct op *
newtp(type)
	int type;
{
	register struct op *t;

	t = (struct op *) alloc(sizeof(*t), ATEMP);
	t->type = type;
	t->u.evalflags = 0;
	t->args = t->vars = NULL;
	t->ioact = NULL;
	t->left = t->right = NULL;
	t->str = NULL;
	return (t);
}

struct op *
compile(s)
	Source *s;
{
	yynerrs = 0;
	multiline.on = s->type == SSTRING;
	multiline.start_token = 0;
	multiline.start_line = 0;
	herep = heres;
	source = s;
	yyparse();
	return outtree;
}

/* This kludge exists to take care of sh/at&t ksh oddity in which
 * the arguments of alias/export/readonly/typeset have no field
 * splitting, file globbing, or (normal) tilde expansion done.
 * at&t ksh seems to do something similar to this since
 *	$ touch a=a; typeset a=[ab]; echo "$a"
 *	a=[ab]
 *	$ x=typeset; $x a=[ab]; echo "$a"
 *	a=a
 *	$
 */
static int
assign_command(s)
	char *s;
{
	char c = *s;

	if (Flag(FPOSIX) || !*s)
		return 0;
	return     (c == 'a' && strcmp(s, "alias") == 0)
		|| (c == 'e' && strcmp(s, "export") == 0)
		|| (c == 'r' && strcmp(s, "readonly") == 0)
		|| (c == 't' && strcmp(s, "typeset") == 0);
}

/* Check if we are in the middle of reading an alias */
static int
inalias(s)
	struct source *s;
{
	for (; s && s->type == SALIAS; s = s->next)
		if (!(s->flags & SF_ALIASEND))
			return 1;
	return 0;
}


#ifdef KSH
/* Order important - indexed by Test_meta values
 * Note that ||, &&, ( and ) can't appear in as unquoted strings
 * in normal shell input, so these can be interpreted unambiguously
 * in the evaluation pass.
 */
static const char dbtest_or[] = { CHAR, '|', CHAR, '|', EOS };
static const char dbtest_and[] = { CHAR, '&', CHAR, '&', EOS };
static const char dbtest_not[] = { CHAR, '!', EOS };
static const char dbtest_oparen[] = { CHAR, '(', EOS };
static const char dbtest_cparen[] = { CHAR, ')', EOS };
const char *const dbtest_tokens[] = {
			dbtest_or, dbtest_and, dbtest_not,
			dbtest_oparen, dbtest_cparen
		};
const char db_close[] = { CHAR, ']', CHAR, ']', EOS };
const char db_lthan[] = { CHAR, '<', EOS };
const char db_gthan[] = { CHAR, '>', EOS };

/* Test if the current token is a whatever.  Accepts the current token if
 * it is.  Returns 0 if it is not, non-zero if it is (in the case of
 * TM_UNOP and TM_BINOP, the returned value is a Test_op).
 */
static int
dbtestp_isa(te, meta)
	Test_env *te;
	Test_meta meta;
{
	int c = tpeek(ARRAYVAR | (meta == TM_BINOP ? 0 : CONTIN));
	int uqword = 0;
	char *save = (char *) 0;
	int ret = 0;

	/* unquoted word? */
	uqword = c == LWORD && *ident;

	if (meta == TM_OR)
		ret = c == LOGOR;
	else if (meta == TM_AND)
		ret = c == LOGAND;
	else if (meta == TM_NOT)
		ret = uqword && strcmp(yylval.cp, dbtest_tokens[(int) TM_NOT]) == 0;
	else if (meta == TM_OPAREN)
		ret = c == '(' /*)*/;
	else if (meta == TM_CPAREN)
		ret = c == /*(*/ ')';
	else if (meta == TM_UNOP || meta == TM_BINOP) {
		if (meta == TM_BINOP && c == REDIR
		    && (yylval.iop->flag == IOREAD
			|| yylval.iop->flag == IOWRITE))
		{
			ret = 1;
			save = wdcopy(yylval.iop->flag == IOREAD ?
				db_lthan : db_gthan, ATEMP);
		} else if (uqword && (ret = (int) test_isop(te, meta, ident)))
			save = yylval.cp;
	} else /* meta == TM_END */
		ret = uqword && strcmp(yylval.cp, db_close) == 0;
	if (ret) {
		ACCEPT;
		if (meta != TM_END) {
			if (!save)
				save = wdcopy(dbtest_tokens[(int) meta], ATEMP);
			XPput(*te->pos.av, save);
		}
	}
	return ret;
}

static const char *
dbtestp_getopnd(te, op, do_eval)
	Test_env *te;
	Test_op op;
	int do_eval;
{
	int c = tpeek(ARRAYVAR);

	if (c != LWORD)
		return (const char *) 0;

	ACCEPT;
	XPput(*te->pos.av, yylval.cp);

	return null;
}

static int
dbtestp_eval(te, op, opnd1, opnd2, do_eval)
	Test_env *te;
	Test_op op;
	const char *opnd1;
	const char *opnd2;
	int do_eval;
{
	return 1;
}

static void
dbtestp_error(te, offset, msg)
	Test_env *te;
	int offset;
	const char *msg;
{
	te->flags |= TEF_ERROR;

	if (offset < 0) {
		REJECT;
		/* Kludgy to say the least... */
		symbol = LWORD;
		yylval.cp = *(XPptrv(*te->pos.av) + XPsize(*te->pos.av)
				+ offset);
	}
	syntaxerr(msg);
}
#endif /* KSH */
