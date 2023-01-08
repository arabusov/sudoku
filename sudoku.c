#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node
{
	int info;
	struct node *p, *s, *d;
};

struct node *head, *z;

void init_tree(void)
{
        int i;
	head = (struct node *)malloc(sizeof(struct node));
	head->p = head;
	z = (struct node *)malloc(sizeof(struct node));
	head->s = z;
	head->d = z;
	z->p = head;
	z->s = z;
	z->d = z;
}

void swp(int *x, int *y)
{
        int tmp = *x;
        *x = *y;
        *y = tmp;
}

void unpack(int info, int *row, int *col, int *box, int *num)
{
	int t;
	t = info % 100;
	*col = t / 10;
	*row = t % 10;
	assert(*col >= 1 && *col <= 9);
	assert(*row >= 1 && *row <= 9);
	*box = ((*row - 1)/3)*3 + (*col - 1)/3 + 1;
	assert(*box>= 1 && *box<= 9);
	*num = info / 100;
	assert(*num >= 1 && *num <= 9);
}

struct node *insert_da(int num, int info, struct node *t)
{
	struct node *x = malloc(sizeof(struct node));
	x->info = info;
	x->s = t->d;
	t->d = x;
	x->d = z;
	x->p = t;
	return x;
}

struct node *insert_sib(int num, int info, struct node *t)
{
	struct node *x = malloc(sizeof(struct node));
	x->info = info;
	x->s = t->s;
	t->s = x;
	x->p = t->p;
	x->d = z;
	return x;
}

int is_valid(struct node *t, int move)
{
	int r, c, b, n;
	int rb, cb, bb, nb;
	unpack(move, &r, &c, &b, &n);
	while (t != head) {
		unpack(t->info, &rb, &cb, &bb, &nb);
		t = t->p;
		if (rb == r && cb == c)
			return 0;
		if (nb == n && (rb == r || cb == c || bb == b))
			return 0;
	}
	return 1;
}

void ind_sort(int *nmoves, int *inds)
{
        int i, j;
        for (i = 0; i < 9; i++) {
                for (j = i + 1; j < 9; j++) {
                        if (nmoves[j] < nmoves[i]) {
                                swp(&nmoves[i], &nmoves[j]);
                                swp(&inds[i], &inds[j]);
                        }
                }
        }
}

struct npmove
{
        int n, ind;
};

int cmp (const void *xx, const void *yy)
{
        struct npmove *x = (struct npmove *)xx;
        struct npmove *y = (struct npmove *)yy;
        if (x->n > y->n)
                return 1;
        else if (x->n == y->n)
                return 0;
        else
                return -1;
}

int immediate(struct node *t)
{
        return 0;
}

int brute_force(struct node *t)
{
	int n, i, j;
        int found_moves = 0;
        int pmoves[9][9*9];
        struct npmove s[9];
        for (n = 1; n <= 9; n++) {
                s[n-1].n = 0;
                s[n-1].ind = n - 1;
                for (i = 1; i <= 9; i++) {
                        for (j = 1; j <= 9; j++) {
                                int move = n*100+10*j+i;
                                if (is_valid(t, move)) {
                                        pmoves[n-1][s[n-1].n++] = move;
                                        found_moves = 1;
                                }
			}
		}
	}
        qsort(s, 9, sizeof(struct npmove), cmp);
        for (n = 0; n < 9; n++) {
                int ind = s[n].ind;
                for (i = 0; i < s[n].n; i++) {
                        insert_da(ind + 1, pmoves[ind][i], t);
                }
        }
        return found_moves;
}

void print_map(struct node *t)
{
        char map[9][10];
        int i, j;
        for (i = 0; i < 9; i ++) {
                for (j = 0; j < 9; j++) {
                        map[i][j] = ' ';
                }
        }
        while (t != head) {
                int r, c, b, n;
                unpack(t->info, &r, &c, &b, &n);
                t = t->p;
                map[r-1][c-1] = n+'0';
        }
        for (i = 0; i < 9; i ++) {
                map[i][9] = '\0';
                printf("%s\n", map[i]);
        }
}

struct node *do_solve_sudoku(struct node *t, int depth, int width)
{
        static int max_depth = 0;
        static int max_width = 0;
        if (depth > max_depth) {
                printf("depth = %d\n", depth);
                max_depth = depth;
                print_map(t);
        }
        if (width > max_width) {
                printf("width = %d\n", width);
                max_width = width;
                print_map(t);
        }
        if (depth == 9*9)
                return t;
        if (immediate(t)) {
                return do_solve_sudoku(t->d, depth + 1, width);
        } else if (brute_force(t)) {
                struct node *d_sltn = do_solve_sudoku (t->d, depth + 1, width);
                if (d_sltn == t->d && t->s != z) {
                        return do_solve_sudoku(t->s, depth, width + 1);
                }
        }
        return t;
}

int n_sudoku_moves(struct node *t)
{
        int n_moves = 0;
        while (t != head) {
                n_moves++;
                t = t->p;
        }
        return n_moves;
}

struct node *solve_sudoku(struct node *t)
{
        int n_moves = n_sudoku_moves(t);
        return do_solve_sudoku(t, n_moves, 1);
}

struct node *make_example(void)
{
        struct node *t;
        t = insert_da(9, 914, head);
        t = insert_da(4, 415, t);
        t = insert_da(5, 517, t);
        t = insert_da(3, 319, t);
        t = insert_da(8, 823, t);
        t = insert_da(3, 325, t);
        t = insert_da(7, 727, t);
        t = insert_da(2, 228, t);
        t = insert_da(4, 432, t);
        t = insert_da(8, 841, t);
        t = insert_da(9, 948, t);
        t = insert_da(2, 251, t);
        t = insert_da(5, 552, t);
        t = insert_da(3, 357, t);
        t = insert_da(8, 867, t);
        t = insert_da(4, 468, t);
        t = insert_da(1, 169, t);
        t = insert_da(4, 471, t);
        t = insert_da(6, 672, t);
        t = insert_da(5, 575, t);
        t = insert_da(8, 876, t);
        t = insert_da(7, 782, t);
        t = insert_da(2, 284, t);
        t = insert_da(6, 686, t);
        t = insert_da(1, 191, t);
        t = insert_da(3, 396, t);
        return insert_da(7, 799, t);
}

int main(void)
{
        struct node *ex;
        init_tree();
        ex = make_example();
        print_map(ex);
        solve_sudoku(ex);
        return 0;
}
