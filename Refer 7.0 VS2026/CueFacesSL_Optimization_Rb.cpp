//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

/* Revision 1.2.  Jim Plank */
/* Original code by Jim Plank (plank@cs.utk.edu) */
/* modified for THINK C 6.0 for Macintosh by Chris Bartley */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

#include "CueFacesSL_Optimization_Rb.h"

static void mk_new_int(Rb_node l, Rb_node r, Rb_node p, int il);
static Rb_node lprev(Rb_node n);
static Rb_node rprev(Rb_node n);
static void recolor(Rb_node n);
static void single_rotate(Rb_node y, int l);
static void rb_print_tree(Rb_node t, int level);
static void rb_iprint_tree(Rb_node t, int level);

 
#define isred(n) (n->s.red)
#define isblack(n) (!isred(n))
#define isleft(n) (n->s.left)
#define isright(n) (!isleft(n))
#define isint(n) (n->s.internal)
#define isext(n) (!isint(n))
#define ishead(n) (n->s.head)
#define isroot(n) (n->s.root)
#define setred(n) n->s.red = 1
#define setblack(n) n->s.red = 0
#define setleft(n) n->s.left = 1
#define setright(n) n->s.left = 0
#define sethead(n) n->s.head = 1
#define setroot(n) n->s.root = 1
#define setint(n) n->s.internal = 1
#define setext(n) n->s.internal = 0
#define setnormal(n) { n->s.root = 0; n ->s.head = 0; }
#define sibling(n) ((isleft(n)) ? n->p.parent->c.child.right \
                                : n->p.parent->c.child.left)
 
static void insert(Rb_node item, Rb_node list)	/* Inserts to the end of a list */
{
  Rb_node last_node;
 
  last_node = list->c.list.blink;
 
  list->c.list.blink = item;
  last_node->c.list.flink = item;
  item->c.list.blink = last_node;
  item->c.list.flink = list;
}

static void delete_item(Rb_node item)		/* Deletes an arbitrary iterm */
{
  item->c.list.flink->c.list.blink = item->c.list.blink;
  item->c.list.blink->c.list.flink = item->c.list.flink;
}

void mk_new_ext(Rb_node *nuevo, char *kkkey, char *vvval, Rb_node aanterior, long ccara,char ppresente) {
  *nuevo = (Rb_node) malloc(sizeof(struct rb_node));
  if (*nuevo==NULL) abort();
  (**nuevo).v.val = vvval;
  (**nuevo).k.key = kkkey;
  (**nuevo).anexo.anterior=aanterior;
  (**nuevo).anexo.cara=ccara;
  (**nuevo).s.presente=ppresente;
  setext((*nuevo));
  setblack((*nuevo));
  setnormal((*nuevo));
}

static void mk_new_int(Rb_node l, Rb_node r, Rb_node p, int il,Rb_node anterior,long cara,char presente)
{
  Rb_node newnode;

  newnode = (Rb_node) malloc(sizeof(struct rb_node));
  if (newnode==NULL) abort();
  setint(newnode);
  setred(newnode);
  setnormal(newnode);
  newnode->c.child.left = l;
  newnode->c.child.right = r;
  newnode->p.parent = p;
  newnode->k.lext = l;
  newnode->v.rext = r;
  newnode->anexo.anterior=anterior;//mio
  newnode->anexo.cara=cara;//mio
  newnode->s.presente=presente;//mio
  l->p.parent = newnode;
  r->p.parent = newnode;
  setleft(l);
  setright(r);
  if (ishead(p)) {
    p->p.root = newnode;
    setroot(newnode);
  } else if (il) {
    setleft(newnode);
    p->c.child.left = newnode;
  } else {
    setright(newnode);
    p->c.child.right = newnode;
  }
  recolor(newnode);
}


Rb_node lprev(Rb_node n)
{
  if (ishead(n)) return n;
  while (!isroot(n)) {
    if (isright(n)) return n->p.parent;
    n = n->p.parent;
  }
  return n->p.parent;
}

Rb_node rprev(Rb_node n)
{
  if (ishead(n)) return n;
  while (!isroot(n)) {
    if (isleft(n)) return n->p.parent;
    n = n->p.parent;
  }
  return n->p.parent;
}

Rb_node make_rb()
{
  Rb_node head;

  head = (Rb_node) malloc (sizeof(struct rb_node));
  if (head==NULL) abort();
  head->c.list.flink = head;
  head->c.list.blink = head;
  head->p.root = head;
  head->k.key = "";
  sethead(head);
  head->anexo.anterior=NULL;//mio
  head->anexo.cara=-1;//mio
  return head;
}


           /* funcion a¤adida por mi */
Rb_node rb_find_min(Rb_node n) {
        Rb_node sig;
        if (!ishead(n)) {
           fprintf(stderr, "rb_find_min called on non-head 0x%x\n", (unsigned int)n->k.ikey);
           return NULL; } //exit(1); }
        sig=n;
        while (sig->c.list.blink!=n) // (sig->c.child.left!=NULL)
              sig=sig->c.child.left;
        return sig;
        }



Rb_node rb_find_ikey_n(Rb_node n, int ikey, int *fnd)
{
  *fnd = 0;
  if (!ishead(n)) {
    fprintf(stderr, "rb_find_ikey_n called on non-head 0x%x\n", (unsigned int)n->k.ikey);
    exit(1);
  }
  if (n->p.root == n) return n;
  if (ikey == n->c.list.blink->k.ikey) {
    *fnd = 1;
    return n->c.list.blink;
  }
  if (ikey > n->c.list.blink->k.ikey) return n;
  else n = n->p.root;
  while (1) {
    if (isext(n)) return n;
    if (ikey == n->k.lext->k.ikey) {
      *fnd = 1;
      return n->k.lext;
    }
    n = (ikey < n->k.lext->k.ikey) ? n->c.child.left : n->c.child.right;
  }
}

Rb_node rb_find_ikey(Rb_node n, int ikey)
{
  int fnd;
  return rb_find_ikey_n(n, ikey, &fnd);
}




Rb_node rb_insert_b(Rb_node n, char *key, char *val,Rb_node anterior,long cara,char presente)
{
  Rb_node newleft, newright, newnode, p;

  if (ishead(n)) {
    if (n->p.root == n) {         /* Tree is empty */
      mk_new_ext(&newnode, key, val,anterior,cara,presente);
      insert(newnode, n);
      n->p.root = newnode;
      newnode->p.parent = n;
      setroot(newnode);
      return newnode;
    } else {
      mk_new_ext(&newright, key, val,anterior,cara,presente);
      insert(newright, n);
      newleft = newright->c.list.blink;
      setnormal(newleft);
      mk_new_int(newleft, newright, newleft->p.parent, isleft(newleft),anterior,cara,presente);
      p = rprev(newright);
      if (!ishead(p)) p->k.lext = newright;
      return newright;
    }
  } else {
    mk_new_ext(&newleft, key, val,anterior,cara,presente);
    insert(newleft, n);
    setnormal(n);
    mk_new_int(newleft, n, n->p.parent, isleft(n),anterior,cara,presente);
    p = lprev(newleft);
    if (!ishead(p)) p->v.rext = newleft;
    return newleft;
  }
}

static void recolor(Rb_node n)
{
  Rb_node p, gp, s;
  int done = 0;

  while(!done) {
    if (isroot(n)) {
      setblack(n);
      return;
    }

    p = n->p.parent;

    if (isblack(p)) return;

    if (isroot(p)) {
      setblack(p);
      return;
    }

    gp = p->p.parent;
    s = sibling(p);
    if (isred(s)) {
      setblack(p);
      setred(gp);
      setblack(s);
      n = gp;
    } else {
      done = 1;
    }
  }
  /* p's sibling is black, p is red, gp is black */

  if ((isleft(n) == 0) == (isleft(p) == 0)) {
    single_rotate(gp, isleft(n));
    setblack(p);
    setred(gp);
  } else {
    single_rotate(p, isleft(n));
    single_rotate(gp, isleft(n));
    setblack(n);
    setred(gp);
  }
}

static void single_rotate(Rb_node y, int l)
{
  int rl, ir;
  Rb_node x, yp;

  ir = isroot(y);
  yp = y->p.parent;
  if (!ir) {
    rl = isleft(y);
  }

  if (l) {
    x = y->c.child.left;
    y->c.child.left = x->c.child.right;
    setleft(y->c.child.left);
    y->c.child.left->p.parent = y;
    x->c.child.right = y;
    setright(y);
  } else {
    x = y->c.child.right;
    y->c.child.right = x->c.child.left;
    setright(y->c.child.right);
    y->c.child.right->p.parent = y;
    x->c.child.left = y;
    setleft(y);
  }

  x->p.parent = yp;
  y->p.parent = x;
  if (ir) {
    yp->p.root = x;
    setnormal(y);
    setroot(x);
  } else {
    if (rl) {
      yp->c.child.left = x;
      setleft(x);
    } else {
      yp->c.child.right = x;
      setright(x);
    }
  }
}

int rb_delete_node(Rb_node n)
{
  Rb_node s, p, gp;
  char ir;

  if (isint(n)) {
    fprintf(stderr, "Cannot delete an internal node: 0x%x\n", (unsigned int)n->k.ikey);
    return -1; //exit(1);
  }
  if (ishead(n)) {
    fprintf(stderr, "Cannot delete the head of an rb_tree: 0x%x\n", (unsigned int)n->k.ikey);
    return -1; //exit(1);
  }
  delete_item(n); /* Delete it from the list */
  p = n->p.parent;  /* The only node */
  if (isroot(n)) {
    p->p.root = p;
    free(n);
    return 1;
  }
  s = sibling(n);    /* The only node after deletion */
  if (isroot(p)) {
    s->p.parent = p->p.parent;
    s->p.parent->p.root = s;
    setroot(s);
    free(p);
    free(n);
    return 1;
  }
  gp = p->p.parent;  /* Set parent to sibling */
  s->p.parent = gp;
  if (isleft(p)) {
    gp->c.child.left = s;
    setleft(s);
  } else {
    gp->c.child.right = s;
    setright(s);
  }
  ir = isred(p);
  free(p);
  free(n);

  if (isext(s)) {      /* Update proper rext and lext values */
    p = lprev(s);
    if (!ishead(p)) p->v.rext = s;
    p = rprev(s);
    if (!ishead(p)) p->k.lext = s;
  } else if (isblack(s)) {
    fprintf(stderr, "DELETION PROB -- sib is black, internal\n");
    return -1; //exit(1);
  } else {
    p = lprev(s);
    if (!ishead(p)) p->v.rext = s->c.child.left;
    p = rprev(s);
    if (!ishead(p)) p->k.lext = s->c.child.right;
    setblack(s);
    return 1;
  }

  if (ir) return 1;

  /* Recolor */

  n = s;
  p = n->p.parent;
  s = sibling(n);
  while(isblack(p) && isblack(s) && isint(s) &&
        isblack(s->c.child.left) && isblack(s->c.child.right)) {
    setred(s);
    n = p;
    if (isroot(n)) return 1;
    p = n->p.parent;
    s = sibling(n);
  }

  if (isblack(p) && isred(s)) {  /* Rotation 2.3b */
    single_rotate(p, isright(n));
    setred(p);
    setblack(s);
    s = sibling(n);
  }

  { Rb_node x, z; char il;

    if (isext(s)) {
      fprintf(stderr, "DELETION ERROR: sibling not internal\n");
      return -1; //exit(1);
    }

    il = isleft(n);
    x = il ? s->c.child.left : s->c.child.right ;
    z = sibling(x);

    if (isred(z)) {  /* Rotation 2.3f */
      single_rotate(p, !il);
      setblack(z);
      if (isred(p)) setred(s); else setblack(s);
      setblack(p);
    } else if (isblack(x)) {   /* Recoloring only (2.3c) */
      if (isred(s) || isblack(p)) {
        fprintf(stderr, "DELETION ERROR: 2.3c not quite right\n");
        return -1; //exit(1);
      }
      setblack(p);
      setred(s);
      return 1;
    } else if (isred(p)) { /* 2.3d */
      single_rotate(s, il);
      single_rotate(p, !il);
      setblack(x);
      setred(s);
      return 1;
    } else {  /* 2.3e */
      single_rotate(s, il);
      single_rotate(p, !il);
      setblack(x);
      return 1;
    }
  }
  return -1;
}


void rb_print_tree(Rb_node t, int level)
{
  int i;
  if (ishead(t) && t->p.parent == t) {
    printf("tree 0x%x is empty\n", (unsigned int)t->k.ikey);
  } else if (ishead(t)) {
    printf("Head: 0x%x.  Root = 0x%x\n", (unsigned int)t->k.ikey, (unsigned int)t->p.root->k.ikey);
    rb_print_tree(t->p.root, 0);
  } else {
    if (isext(t)) {
      for (i = 0; i < level; i++) putchar(' ');
      printf("Ext node 0x%x: %c,%c: p=0x%x, k=%s\n",
		  (unsigned int)t->k.ikey,
		  (unsigned int)isred(t)?'R':'B',
		  (unsigned int)isleft(t)?'l':'r',
		  (unsigned int)t->p.parent->k.ikey,
		  t->k.key);
    } else {
      rb_print_tree(t->c.child.left, level+2);
      rb_print_tree(t->c.child.right, level+2);
      for (i = 0; i < level; i++) putchar(' ');
      printf("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%s,%s)\n",
		  (unsigned int)t->k.ikey,
		  (unsigned int)isred(t)?'R':'B',
		  (unsigned int)isleft(t)?'l':'r',
		  (unsigned int)t->c.child.left->k.ikey,
		  (unsigned int)t->c.child.right->k.ikey,
		  (unsigned int)t->p.parent->k.ikey,
		  t->k.lext->k.key,
		  t->v.rext->k.key);
    }
  }
}

void rb_iprint_tree(Rb_node t, int level)
{
  int i;
  if (ishead(t) && t->p.parent == t) {
    printf("tree 0x%x is empty\n", (unsigned int)t->k.ikey);
  } else if (ishead(t)) {
    printf("Head: 0x%x.  Root = 0x%x, < = 0x%x, > = 0x%x\n",
			(unsigned int)t->k.ikey,
			(unsigned int)t->p.root->k.ikey,
			(unsigned int)t->c.list.blink->k.ikey,
			(unsigned int)t->c.list.flink->k.ikey);
    rb_iprint_tree(t->p.root, 0);
  } else {
    if (isext(t)) {
      for (i = 0; i < level; i++) putchar(' ');
      printf("Ext node 0x%x: %c,%c: p=0x%x, <=0x%x, >=0x%x k=%d\n",
				(unsigned int)t->k.ikey,
				(unsigned int)isred(t)?'R':'B',
				(unsigned int)isleft(t)?'l':'r',
				(unsigned int)t->p.parent->k.ikey,
				(unsigned int)t->c.list.blink->k.ikey,
				(unsigned int)t->c.list.flink->k.ikey,
				t->k.ikey);
    } else {
      rb_iprint_tree(t->c.child.left, level+2);
      rb_iprint_tree(t->c.child.right, level+2);
      for (i = 0; i < level; i++) putchar(' ');
      printf("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%d,%d)\n",
				(unsigned int)t->k.ikey,
				(unsigned int)isred(t)?'R':'B',
				(unsigned int)isleft(t)?'l':'r',
				(unsigned int)t->c.child.left->k.ikey,
				(unsigned int)t->c.child.right->k.ikey,
				(unsigned int)t->p.parent->k.ikey,
				t->k.lext->k.ikey,
				t->v.rext->k.ikey);
    }
  }
}
 
void rb_free_tree(Rb_node n)
{
  if (!ishead(n)) {
    fprintf(stderr, "ERROR: Rb_free_tree called on a non-head node\n");
    exit(1);
  }

  while(rb_first(n) != rb_nil(n)) {
    rb_delete_node(rb_first(n));
  }
  free(n);
}

Rb_node rb_inserti(Rb_node tree, int ikey, char *val, Rb_node anterior, long cara, char presente)
{
  //return rb_insert_b(rb_find_ikey(tree, ikey), (char *) ikey, val, anterior, cara, presente);

  //Convert integer to std::string
  //std::string str = std::to_string(ikey);
  std::stringstream ss;
  ss << ikey;
  std::string str = ss.str();

  //Convert std::string into a char*
  //char* c_ikey = str.data();
  std::vector<char> str_copy(str.c_str(), str.c_str() + str.size() + 1);
  //char* c_ikey = str_copy.data();
  char* c_ikey = &str_copy[0];

  return rb_insert_b(rb_find_ikey(tree, ikey), c_ikey, val, anterior, cara, presente);
}
