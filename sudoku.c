#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node
{
	int info;
        int nums[9], stat[9];
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
        for (i = 0; i < 9; i ++) {
                z->nums[i] = head->nums[i] = i + 1;
                z->stat[i] = head->stat[i] = 0;
        }
}

void swp(int *x, int *y)
{
        int tmp = *x;
        *x = *y;
        *y = tmp;
}

void bubble_num(struct node *x, int j)
{
        int i;
        for (i = j-1; i >= 0; i--) {
                if (x->stat[j] > x->stat[i]) {
                        swp(&x->stat[j], &x->stat[i]);
                        swp(&x->nums[j], &x->nums[i]);
                        j = i;
                }
        }
}

int copy_stat(int num, struct node *x, struct node *t)
{
        int j = -1, i;
        for (i = 0; i < 9; i++) {
                x->nums[i] = t->nums[i];
                x->stat[i] = t->stat[i];
                if (num == x->nums[i])
                        j = i;
        }
        return j;
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
        int j=-1;
	struct node *x = malloc(sizeof(struct node));
        j = copy_stat(num, x, t);
        assert(j > -1);
        x->stat[j]++;
        bubble_num(x, j);
	x->info = info;
	x->s = t->d;
	t->d = x;
	x->d = z;
	x->p = t;
	return x;
}

struct node *insert_sib(int num, int info, struct node *t)
{
        int  j=-1;
	struct node *x = malloc(sizeof(struct node));
	x->info = info;
	x->s = t->s;
	t->s = x;
	x->p = t->p;
        copy_stat(num, x, t->p);
        assert(j > -1);
        x->stat[j]++;
        bubble_num(x, j);
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

int find_moves(struct node *t)
{
	int k, i, j;
        int found_moves = 0;
        for (k = 1; k <= 9; k++) {
                int n = t->nums[k-1];
                assert(n > 0 && n <= 9);
                assert(t->stat[k-1] >= 0 && t->stat[k-1] <= 9);
                if (t->stat[k-1] == 9)
                        continue;
                for (i = 1; i <= 9; i++) {
                        for (j = 1; j <= 9; j++) {
				int move;
				move = n*100+10*j+i;
				if (is_valid(t, move)) {
					insert_da(n, move, t);
                                        found_moves = 1;
                                }
			}
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
        if (find_moves(t)) {
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

int main(void)
{
        init_tree();
        solve_sudoku(head);
        return 0;
}
