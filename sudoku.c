#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node
{
	int info;
        int depth;
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

struct node *insert_da(int info, struct node *t)
{
	struct node *x = malloc(sizeof(struct node));
	x->info = info;
	x->s = t->d;
	t->d = x;
	x->d = z;
	x->p = t;
	return x;
}

struct node *insert_sib(int info, struct node *t)
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

int check_rows(struct node *t, int (*map)[9], int (*calc_move)(int, int))
{
        int i, j, n;
        for (i = 0; i < 9; i++) {
                for (n = 1; n <= 9; n++) {
                        int nmoves = 0;
                        int pmove = 0;
                        for (j = 0; j < 9; j++) {
                                int move = n*100 + (*calc_move)(i, j);
                                if (is_valid(t, move)) {
                                        nmoves++;
                                        pmove = move;
                                }
                        }
                        if (nmoves == 1) {
                                insert_da(pmove, t);
                                return 1;
                        }
                }
        }
        return 0;
}

int row_major(int i, int j)
{
        return i + 1 + (j + 1)*10;
}

int col_major(int i, int j)
{
        return j + 1 + (i + 1)*10;
}

void fill_map(struct node *t, int (*map)[9])
{
        int i, j;
        for (i = 0; i < 9; i++) {
                for (j = 0; j < 9; j++) {
                        map[i][j] = 0;
                }
        }
        while (t != head) {
                int r, c, b, n;
                unpack(t->info, &r, &c, &b, &n);
                t = t->p;
                map[r-1][c-1] = n;
        }
}

int immediate(struct node *t)
{
        int map[9][9];
        fill_map(t, map);
        return check_rows(t, map, &row_major) || check_rows(t, map, &col_major);
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
                        insert_da(pmoves[ind][i], t);
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
        printf("+---------+\n");
        for (i = 0; i < 9; i ++) {
                map[i][9] = '\0';
                printf("|%s|\n", map[i]);
        }
        printf("+---------+\n");
}

struct node *do_solve_sudoku(struct node *t, int depth, int width)
{
        static int max_depth = 0;
        static int max_width = 0;
        t->depth = depth;
        if (depth > max_depth) {
                max_depth = depth;
        }
        if (width > max_width) {
                max_width = width;
        }
        if (depth == 9*9) {
                return t;
        }
        if (immediate(t)) {
                return do_solve_sudoku(t->d, depth + 1, width);
        } else {
                if (brute_force(t)) {
                        struct node *da = t->d;
                        int w = width;
                        while (da != z) {
                                struct node *sl = do_solve_sudoku(da, depth + 1, ++w);
                                da = da->s;
                                if (sl->depth == 9*9)
                                        return sl;
                        }
                }
        }
        return z;
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
        t = insert_da(941, head);
        t = insert_da(451, t);
        t = insert_da(571, t);
        t = insert_da(391, t);
        t = insert_da(832, t);
        t = insert_da(352, t);
        t = insert_da(772, t);
        t = insert_da(282, t);
        t = insert_da(423, t);
        t = insert_da(814, t);
        t = insert_da(984, t);
        t = insert_da(215, t);
        t = insert_da(525, t);
        t = insert_da(375, t);
        t = insert_da(876, t);
        t = insert_da(486, t);
        t = insert_da(196, t);
        t = insert_da(417, t);
        t = insert_da(627, t);
        t = insert_da(557, t);
        t = insert_da(867, t);
        t = insert_da(728, t);
        t = insert_da(248, t);
        t = insert_da(668, t);
        t = insert_da(119, t);
        t = insert_da(369, t);
        return insert_da(799, t);
}

int main(void)
{
        struct node *ex, *sol;
        init_tree();
        ex = make_example();
        print_map(ex);
        sol = solve_sudoku(ex);
        print_map(sol);
        return 0;
}
