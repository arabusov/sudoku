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

int check_rows(struct node *t, int (*calc_move)(int, int))
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

int immediate(struct node *t)
{
        return check_rows(t, &row_major) || check_rows(t, &col_major);
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

void read_map(int (*map)[9])
{
        int i = 0, j = 0, ch;
        while (ch = getchar(), ch != EOF) {
                if (ch == '\n') {
                        assert(i < 9);
                        i++;
                        j = 0;
                } else {
                        assert(j < 9);
                        if (ch == ' ') {
                                map[i][j] = 0;
                        } else {
                                assert(ch >= '0' && ch <= '9');
                                map[i][j] = ch-'0';
                        }
                        j++;
                }
        }
}

struct node *map2tree(int (*map)[9])
{
        int i, j;
        struct node *t = head;
        for (i = 0; i < 9; i++) {
                for (j = 0; j < 9; j++) {
                        if (map[i][j] > 0) {
                                int move = 100*map[i][j] + i + 1 + 10*(j + 1);
                                t = insert_da(move, t);
                        }
                }
        }
        return t;
}

struct node *read_stdin(void)
{
       int map[9][9];
       read_map(map);
       return map2tree(map);
}

int main(void)
{
        struct node *pr, *sol;
        init_tree();
        pr = read_stdin();
        print_map(pr);
        sol = solve_sudoku(pr);
        print_map(sol);
        return 0;
}
