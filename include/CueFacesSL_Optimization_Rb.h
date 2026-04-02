/* Revision 1.2.  Jim Plank */

/* Original code by Jim Plank (plank@cs.utk.edu) */
/* modified for THINK C 6.0 for Macintosh by Chris Bartley */


#ifndef FACES_SL_OPTIMIZATION_VISITED
	#define FACES_SL_OPTIMIZATION_VISITED

	struct status {
	  unsigned red : 1;
	  unsigned internal : 1;
	  unsigned left : 1;
	  unsigned root : 1;
	  unsigned head : 1;
	  unsigned presente : 1;
	};

	/* Main rb_node.  You only ever use the fields

	   c.list.flink
	   c.list.blink

	   k.key or k.ikey
	   v.val
	*/


	typedef struct rb_node {
	  union {
		struct {
		  struct rb_node *flink;
		  struct rb_node *blink;
		} list;
		struct {
		  struct rb_node *left;
		  struct rb_node *right;
		} child;
	  } c;
	  union {
		struct rb_node *parent;
		struct rb_node *root;
	  } p;
	  status s;
	  union {
		int ikey;
		char *key;
		struct rb_node *lext;
	  } k;
	  union {
		char *val;
		struct rb_node *rext;
	  } v;
	  struct {
			struct rb_node *anterior;
			long cara;
			} anexo;
	} *Rb_node;


	void rb_iprint_tree(Rb_node t, int level);

	Rb_node rb_find_ikey(Rb_node n, int ikey);

	//public
	Rb_node make_rb();
	Rb_node rb_find_min(Rb_node n);
	int rb_delete_node(Rb_node n);
	void rb_free_tree(Rb_node n);
	Rb_node rb_inserti(Rb_node tree, int ikey, char *val,Rb_node anterior,long cara,char presente);
	// private

	#define rb_first(n) (n->c.list.flink)
	#define rb_last(n) (n->c.list.blink)
	#define rb_next(n) (n->c.list.flink)
	#define rb_prev(n) (n->c.list.blink)
	#define rb_empty(t) (t->c.list.flink == t)
	#ifndef rb_nil
	#define rb_nil(t) (t)
	#endif

	#define rb_traverse(ptr, lst) \
	  for(ptr = rb_first(lst); ptr != rb_nil(lst); ptr = rb_next(ptr))
 
#endif //FACES_SL_OPTIMIZATION_VISITED
