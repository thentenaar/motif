/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/

#include <Xm/PictureP.h>

static XmPictureNode* _XiGetNewNode(XmPictureRec*);
static void _XmPictureParseNode(XmPictureRec*, char**, XmPictureNode**,
				XmPictureNode**, Boolean);
static XmPictureTransition* _XiGetNewTransition(XmTransType,
						XmPictureNode*,
						XmPictureNode*);
static void _XmPictureSetState(unsigned char*, int);
static char _XmPictureGetState(unsigned char*, int);
static void _XmPictureFollowTransitions(XmPictureStateRec*, char, XmPictureNode*);
static XmPictureNode *_XmPictureCopySubGraph(XmPictureRec*, int,
					     XmPictureNode*, XmPictureNode*);
static void _XmPictureTagNodes(XmPictureRec*, XmPictureNode**, int);
static void _XmPictureFillTraverse(XmPictureRec*, int, XmAutoFill*);

/*
 * Parses the given string into an XmPicture object.  Returns NULL on a
 * mal-formed picture
 */
XmPicture
XmParsePicture(char *input)
{
    XmPictureRec *picture;
    XmPictureNode *root_node;
    XmPictureNode *end_node;

    picture = XtNew(XmPictureRec);

    picture->source = XtNewString(input);
    picture->num_nodes = 0;
    picture->nodes_alloced = NODE_START_COUNT;
    picture->nodes = (XmPictureNode**)
	             XtMalloc(NODE_START_COUNT * sizeof(XmPictureNode*));

    _XmPictureParseNode(picture, &input, &root_node, &end_node, False);

    picture->start_node = root_node->index;
    picture->final_node = end_node->index;
    
    return picture;
}

/*
 * Creates a new XmPictureState.  The memory allocated must be freed by the
 * user with XtFree()
 */
XmPictureState
XmGetNewPictureState(XmPicture picture)
{
    int i;
    XmPictureStateRec *state;

    state = XtNew(XmPictureStateRec);

    state->statesize = 1 + (picture->num_nodes * sizeof(char) / 8);

    state->picture = picture;

    state->state = (unsigned char*) XtMalloc(state->statesize);
    state->newstate = (unsigned char*) XtMalloc(state->statesize);
    for(i=0; i<state->statesize; i++) {
	state->state[i] = 0;
	state->newstate[i] = 0;
    }

    _XmPictureSetState(state->state, picture->start_node);

    /*
     * BAD BAD BAD HACK
     * This just allocates a static 1024 character array.  While this will
     * be fine for the DataField, because only one state per widget will
     * be allocated and because the strings will never get that long,
     * this makes this library useless for generalized database stuff.
     * DON'T attempt to make lots of these or use them for very long
     * regexps until I get this fixed -- Andy
     * Sigh, additionally, this is a security problem.  Feeding this
     * thing very long RE's and strings will therefore make the
     * program crash, which is not something our customers want
     */
    state->current_string = XtMalloc(1024 * sizeof(char));
    state->current_string[0] = '\0';
    state->append = state->current_string;

    return state;
}

/*
 * Processes a single character.  Returns a pointer to the current
 * value of the string (which may have been auto-filled, if
 * do_auto_fill is specified).  If the current state vector contains
 * the final state, it places True in is_finished, otherwise False.
 */
char*
XmPictureProcessCharacter(XmPictureState state, char in, Boolean *is_finished)
{
    int i;
    char status;
    unsigned char *temp;
    char *save_start;

    /*
     * Reset the current-processing info
     */
    state->current = '\0';
    state->upcase  = False;
    for(i=0; i<state->statesize; i++)
	state->newstate[i] = 0;

    /*
     * For each node in the state set, try to follow all transitions
     * (recursing on NullTransitions of course)
     */    
    for(i=0; i<state->picture->num_nodes; i++) {
	if(_XmPictureGetState(state->state, i)) {
	    _XmPictureFollowTransitions(state, in, state->picture->nodes[i]);
	}
    }

    /*
     * Swap the states
     */
    temp = state->state;
    state->state = state->newstate;
    state->newstate = temp;

    /*
     * Append whatever we accepted (which might have been upcased)
     */
    save_start = state->append;
    if(state->current) {
	*state->append = state->current;
	state->append++;
	*state->append = '\0';
    }

    /*
     * If we didn't find _any_ transitions, return NULL
     */
    for(i=0; i<state->statesize; i++)
	if(state->state[i] != 0)
	    break;
    if(i == state->statesize) {
	*is_finished = True;
	return NULL;
    }

    /*
     * If our set contains the final state, set is_finished to true
     */
    status = _XmPictureGetState(state->state, state->picture->final_node);
    if(status) *is_finished = True;
    else *is_finished = False;

    return save_start;
}

void
XmPictureDelete(XmPicture p)
{
    int i;
    XmPictureTransition *trans, *delete_me;

    /*
     * First walk all the nodes, deleting the transitions then the
     * nodes themselves
     */
    for(i=0; i<p->num_nodes; i++) {
	trans = p->nodes[i]->transitions;
	while(trans) {
	    delete_me = trans;
	    trans = trans->next;
	    XtFree((char*)delete_me);
	}
	XtFree((char*)p->nodes[i]);
    }

    /*
     * Now the node table and finally the picture itself
     */
    XtFree((char*)p->nodes);
    XtFree(p->source);
    XtFree((char*)p);
}

void
XmPictureDeleteState(XmPictureState s)
{
    XtFree(s->current_string);
    XtFree((char*)s->state);
    XtFree((char*)s->newstate);
    XtFree((char*)s);
}


char*
XmPictureGetCurrentString(XmPictureState s)
{
    return s->current_string;
}

char*
XmPictureDoAutoFill(XmPictureState state)
{
    XmAutoFill fill;
    int i;
    Boolean finished = False;

    while(1) {

	fill.reject = False;
	fill.c      = '\0';
	fill.digit  = False;
	fill.upcase = False;
	fill.letter = False;
	fill.hexdigit = False;
	fill.octaldigit = False;

	for(i=0; i<state->picture->num_nodes; i++)
	    if(_XmPictureGetState(state->state, i))
		_XmPictureFillTraverse(state->picture, i, &fill);

	if(fill.c == '\0')
	    fill.reject = True;
	if(fill.digit && (isdigit(fill.c) == 0))
	    fill.reject = True;
	if(fill.hexdigit && (isxdigit(fill.c) == 0))
	    fill.reject = True;
	if(fill.octaldigit && (fill.c < '0' || fill.c > '7'))
	    fill.reject = True;
	if(fill.letter && (isalpha(fill.c) == 0))
	    fill.reject = True;
	if(fill.upcase && islower(fill.c))
	    fill.reject = True;

	if(fill.reject) return state->current_string;

	XmPictureProcessCharacter(state, fill.c, &finished);

	if(finished) return state->current_string;
    }

}

/*
 * Parses a single "node" of the regular expression.  If returnNOW is true,
 * this means only the very first complete RE encountered (a hack, to allow
 * for the weird closure precedence:  *ab should parse like {*a}b and not
 * *{ab}), otherwise it reads until the end of the RE, be it a close brace
 * or EOS.
 */
static void
_XmPictureParseNode(XmPictureRec *picture, char **in_string,
		    XmPictureNode **start_return,
		    XmPictureNode **end_return,
		    Boolean returnNOW)
{
    XmPictureNode *start_node;
    XmPictureNode *current_node;
    XmPictureNode *start_node_2;
    XmPictureNode *current_node_2;
    XmPictureNode *newnode;
    int node_idx;
    XmPictureTransition *newtrans;
    char inc;
    int count;
    char *endcount;

    start_node = _XiGetNewNode(picture);
    node_idx = start_node->index;
    current_node = start_node;

    while((inc = *((*in_string)++))) {
	switch(inc) {
	    /*
	     * These are the "normal" single-character tokens that
	     * behave (more or less) like literals in the state
	     * machine.
	     */
	case NUMDIGIT:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(NumericDigit,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case HEXDIGIT:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(HexDigit,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case OCTALDIGIT:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(OctalDigit,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case NONCASELETTER:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(AnyLetter,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case UPCASELETTER:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(UpCaseLetter,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case NONCASECHAR:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(AnyCharacter,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	case UPCASECHAR:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(UpCaseCharacter,
					   current_node, newnode);
	    current_node = newnode;
	    break;
	    
	    /*
	     * The weird stuff.
	     */
	    
	    /*
	     * Read a numeric specifier, if it exists, then read the
	     * next _single_ R.E. (i.e., not a string of concatenated
	     * ones!); Add a transition from the current end state to the
	     * end (!) state of the new closure states, and a transition
	     * from that end to it's beginning.
	     */
	case CLOSURE: /* CR03602 picture should formatted to *nx;
			n is no. of time repeat; x is the specified
			character going to be display */
	    count = strtol(*in_string, &endcount, 0);
	    if(count)
		*in_string = endcount;		
	    _XmPictureParseNode(picture, in_string,
				&start_node_2, &current_node_2,
				True);
	    if(count) {
		newtrans = _XiGetNewTransition(NullTransition,
					       current_node, start_node_2);
		current_node = _XmPictureCopySubGraph(picture, count-1,
						      start_node_2,
						      current_node_2);
	    } else {
		newtrans = _XiGetNewTransition(NullTransition,
					       current_node_2, start_node_2);
		newtrans = _XiGetNewTransition(NullTransition,
					       current_node, current_node_2);
		
		current_node = current_node_2;
	    }
	    break;
	    
	    /*
	     * Read the next (NOT singleton, this time) R.E.; create a _new_
	     * initial state which has null-transitions to both the
	     * current and new R.E.'s, and a new final state to which
	     * both trails lead.
	     */
	case ALTERNATIVE: /* CR03656 CR03359 a major overhaul */
	    _XmPictureParseNode(picture, in_string,
				&start_node_2, &current_node_2,
				True);
            newtrans = _XiGetNewTransition(NullTransition, current_node,
					   current_node_2);
            if (current_node->index)
              picture->nodes[current_node->index - 1]->transitions->next =
		start_node_2->transitions;
            current_node = current_node_2;
	    break;
	    
	    /*
	     * This actually is a special case of ALTERNATIVE.  The
	     * stuff inside the brackets becomes one chain, and a
	     * null-transition becomes the second.  Just read the
	     * (NON-singleton) RE until the close-bracket.
	     */
	case LBRACKET:
	    _XmPictureParseNode(picture, in_string,
				&start_node_2, &current_node_2,
				False);
	    
	    newtrans = _XiGetNewTransition(NullTransition,
					   current_node, current_node_2);
	    newtrans = _XiGetNewTransition(NullTransition,
					   current_node, start_node_2);

	    current_node = current_node_2;

	    break;
	    
	    /*
	     * The easiest special case to handle.  Just read a
	     * non-singleton R.E. until the close-brace and stick it
	     * in as a "literal".
	     */
	case LBRACE:
	    _XmPictureParseNode(picture, in_string,
				&start_node_2, &current_node_2,
				False);

	    newtrans = _XiGetNewTransition(NullTransition,
					   current_node, start_node_2);
	    
	    current_node = current_node_2;

	    break;
	    
	    /*
	     * Deal with both end-of-subexpression tokens in the same
	     * way.  This will accept stuff like {...] and [...}, but that's
	     * probably OK, since it will only parse stuff incorrectly
	     * for things like: [...{...]...} which aren't legal
	     * anyway.  I.E., we accept a superset of legal pictures,
	     * but parse all legal pictures correctly.
	     */
	case RBRACE:
	case RBRACKET:
	    returnNOW = True;
	    break;
	    
	    /*
	     * Special case handling for the ';' escape character.
	     * Make sure dumb users haven't escaped the end of the string
	     * then advance the pointer and fall through to the (default)
	     * handling for a literal.
	     */
	case ESCAPE:
	    inc = **in_string;
	    if(inc == '\0') break;
	    else (*in_string)++;
	    /*
	     * It's not a special character, so it must be a literal
	     */
	default:
	    newnode = _XiGetNewNode(picture);
	    newtrans = _XiGetNewTransition(LiteralCharacter,
					   current_node, newnode);
	    newtrans->c = inc;
	    current_node = newnode;
	    break;
	}

	/*
	 * Break out if we have to
	 */
	if(returnNOW == True) break;
    }
    *start_return = start_node;
    *end_return = current_node;
}

static XmPictureTransition*
_XiGetNewTransition(XmTransType type,
		    XmPictureNode *src, XmPictureNode *dest)
{
    XmPictureTransition *t = XtNew(XmPictureTransition);
    t->destination = dest->index;
    t->type = type;
    t->next = src->transitions;
    src->transitions = t;
    return t;
}

/*
 * Allocates a new state machine node
 */
static XmPictureNode*
_XiGetNewNode(XmPictureRec *picture)
{
    XmPictureNode *new_node;

    new_node = XtNew(XmPictureNode);
    new_node->transitions = NULL;
    new_node->index = picture->num_nodes++;

    /*
     * Handle growing the picture past it's boundary
     */
    if(picture->num_nodes > picture->nodes_alloced) {
	int newsize = picture->nodes_alloced * 2;

	picture->nodes = (XmPictureNode**)
	                 XtRealloc((char*)picture->nodes,
				   newsize * sizeof(XmPictureNode*));
	picture->nodes_alloced = newsize;
    }

    picture->nodes[new_node->index] = new_node;

    return new_node;
}

static void
_XmPictureSetState(unsigned char *v, int i)
{
    int base = i / 8;
    int offset = i % 8;

    v[base] |= 1 << offset;
}

/*
 * Do I need this at all?
 *
static void
_XmPictureClearState(char *v, int i)
{
    int base = i / 8;
    int offset = i % 8;

    v[base] &= ~(1 << offset);
}
*/

static char
_XmPictureGetState(unsigned char *v, int i)
{
    int base = i / 8;
    int offset = i % 8;
    int result = v[base] & (1 << offset);

    if(result) return 1;
    else return 0;
}

/*
 * The inc parameter gets set to '\0' if we are following a null
 * transition after already having read the character.
 */
static void
_XmPictureFollowTransitions(XmPictureStateRec *state, char inc,
			    XmPictureNode *node)
{
    XmPictureTransition *curr = node->transitions;
    char follow_c, changed_c;
    Boolean found = False;
    Boolean accepted = True;

    while(curr) {
	follow_c = '\0';
	changed_c = '\0';
	switch(curr->type) {
	case NullTransition:
	    follow_c = inc;
	    found = True;
	    if(inc != '\0')
		accepted = False;
	    break;
	case NumericDigit:
	    if(isdigit(inc)) {
		found = True;
	    }
	    break;
	case HexDigit:
	    if(isdigit(inc) ||
		(inc >= 'a' && inc <= 'f') ||
		(inc >= 'A' && inc <= 'F')) {
		found = True;
	    }
	    break;
	case OctalDigit:
	    if((inc >= '0') && (inc <= '7')) {
		found = True;
	    }
	    break;
	case AnyLetter:
	    if(isalpha(inc)) {
		found = True;
	    }
	    break;
	case UpCaseLetter:
	    if(isalpha(inc)) {
		changed_c = toupper(inc);
		found = True;
	    }
	    break;
	case AnyCharacter:
	    if(isalnum(inc)) {
		found = True;
	    }
	    break;
	case UpCaseCharacter:
	    if(isalnum(inc)) {
		changed_c = toupper(inc);
		found = True;
	    }
	    break;
	case LiteralCharacter:
	    if(inc == curr->c) {
		found = True;
	    }
	    break;
	}

	if(found) {
	    if(changed_c) {
		state->current = changed_c;
	    } else if(inc) {
		state->current = inc;
	    }
	    if(accepted)
		_XmPictureSetState(state->newstate, curr->destination);
	    _XmPictureFollowTransitions(state, follow_c,
					state->picture
					  ->nodes[curr->destination]);
	}
	found = False;
	curr = curr->next;
    }
}


/*
 * Recursively traverses a subgraph, tagging all indices in table with
 * a non-null value.
 */
static void
_XmPictureTagNodes(XmPictureRec *picture, XmPictureNode **table, int start)
{
    XmPictureTransition *trans = picture->nodes[start]->transitions;

    table[start] = (XmPictureNode*)0x1;

    while(trans) {
	_XmPictureTagNodes(picture, table, trans->destination);
	trans = trans->next;
    }
}

/*
 * Makes a string of n copies of the subgraph indicated, chaining them
 * end-to-end.  It returns the new end node for the multiplied
 * subgraph (the start node is the unchanged, of course)
 */
static XmPictureNode*
_XmPictureCopySubGraph(XmPictureRec *picture, int n,
		       XmPictureNode *start, XmPictureNode *end)
{
    XmPictureNode **table;
    XmPictureTransition *trans, *newtrans;
    int i, j, tablesize, end_index;


    /*
     * Bail if the user did a repeat count of 1
     */
    if(n < 1) return end;
    
    /*
     * First allocate a pointer array of the same size as the current
     * picture.  This will hold NULL for nodes not in the subgraph and
     * the XmPictureNode for the newest copy.
     */
    tablesize = picture->num_nodes;
    table = (XmPictureNode**)
	    XtMalloc(tablesize * sizeof(XmPictureNode*));
    for(i=0; i<picture->num_nodes; i++)
	table[i] = NULL;
    
    /*
     * Now recursively tag the ones in our subgraph with non-NULL
     * values
     */
    _XmPictureTagNodes(picture, table, start->index);

    /*
     * Now loop:  for each non-NULL, allocate a new node in its
     * place; then go through each transition from the _original_
     * nodes, and put equivalent ones in the new table's nodes.
     * Finally chain it to the end of the previous node.
     */
    end_index = end->index;
    for(i=0; i<n; i++) {

	/* Allocate the new nodes */
	for(j=0; j<tablesize; j++)
	    if(table[j]) table[j] = _XiGetNewNode(picture);

	/* Fill them in */
	for(j=0; j<tablesize; j++) {
	    if(table[j]) {
		trans = picture->nodes[j]->transitions;
		while(trans) {
		    /*
		     * Hack to make sure we don't follow transitions
                     * into the newly created nodes
		     */
		    if(trans->destination >= tablesize) {
			trans = trans->next;
			continue;
		    }

		    newtrans = _XiGetNewTransition(trans->type,
						   table[j],
						   table[trans->destination]);
		    newtrans->c = trans->c;
		    trans = trans->next;
		}
	    }
	}

	_XiGetNewTransition(NullTransition, end, table[start->index]);
	end = table[end_index];
    }
    
    XtFree((void*)table);

    return end;
}

static void
_XmPictureFillTraverse(XmPictureRec *picture, int start, XmAutoFill *fill)
{
    XmPictureTransition *trans = picture->nodes[start]->transitions;

    while(trans) {
	switch(trans->type) {
	case NullTransition:
	    _XmPictureFillTraverse(picture, trans->destination, fill);
	    break;
	case NumericDigit:
	    fill->digit = True;
	    break;
	case HexDigit:
	    fill->hexdigit = True;
	    break;
	case OctalDigit:
	    fill->octaldigit = True;
	    break;
	case AnyLetter:
	    fill->letter = True;
	    break;
	case UpCaseLetter:
	    fill->letter = True;
	    fill->upcase = True;
	    break;
	case UpCaseCharacter:
	    fill->upcase = True;
	    break;
	case LiteralCharacter:
	    if(fill->c == '\0')
		fill->c = trans->c;
	    else if(fill->c != trans->c)
		fill->reject = True;
	    break;
	    /* gr... AnyCharacter isn't handled, and gcc issues a warning. */
	default:
	    break;
	}
	trans = trans->next;
    }
}

#ifdef DEBUG
static void
_XiPrintStateVector(XmPictureStateRec *state)
{
    int i;
    char c;

    for(i=0; i<state->picture->num_nodes; i++) {
	if(_XmPictureGetState(state->state, i))
	    c = '1';
	else
	    c = '0';
	printf("%c", c);
    }
    printf("\n");
}
#endif
