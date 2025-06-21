/* ************************************************************************** */
/*                                                                            */
/*                                                    _.._  .           .     */
/*   shnake.c                                       .' .-'`        *          */
/*                                                 /  /       +        *      */
/*   By: ldel-val <ldel-val@student.42madrid.com>  |  |           *           */
/*                                                 \  '.___.;       +         */
/*   Created: 2025/05/30 22:25:17 by ldel-val       '._  _.'   .        .     */
/*   Updated: 2025/06/02 14:34:43 by ldel-val          ``                     */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>

void	set_terminal_mode(int mode)
{
	static struct termios	oldt;
	static struct termios	newt;

	if (mode == 0)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	}
	else
	{
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	}
}

#define D_RIGHT 0
#define D_UP 1
#define D_LEFT 2
#define D_DOWN 3
#define GREEN "\001\x1B[32m\002"
#define YELLOW "\001\x1B[33m\002"
#define MAGENTA "\001\x1B[35m\002"
#define RESET "\001\x1B[0m\002"
#define CURSOR_UP_16 "\033[16A"

int get_head_direction(int	current)
{
	char	buf[64];
	int		n;
	int		direction;

	ioctl(STDIN_FILENO, FIONREAD, &n);
	if (n <= 0)
		return (current);
	n = read(STDIN_FILENO, buf, 128);
	if (n >= 3 && buf[0] == 27 && buf[1] == 91)
	{
		if (buf[2] == 65 && current != D_DOWN)
			return (D_UP);
		else if (buf[2] == 66 && current != D_UP)
			return (D_DOWN);
		else if (buf[2] == 67 && current != D_LEFT)
			return (D_RIGHT);
		else if (buf[2] == 68 && current != D_RIGHT)
			return (D_LEFT);
	}
	return (current);
}

void	busy_wait(unsigned long int units)
{
	volatile unsigned long int i;

	i = 0;
	while (i < units)
		i++;
	return ;
}

typedef struct	s_snake
{
	int		apple[2];
	int		snake_len;
	int		snake_direction;
	int		body[64][2];
}	t_snake;

#define X_POS 0
#define Y_POS 1
#define DIRECTION 2

t_snake	*init_snake(void)
{
	t_snake	*snake;

	snake = malloc(sizeof(t_snake));
	snake->snake_len = 1;
	snake->body[0][X_POS] = 8;
	snake->body[0][Y_POS] = 8;
	snake->snake_direction = D_UP;
	snake->apple[X_POS] = 9;
	snake->apple[Y_POS] = 3;
	return (snake);
}

void print_map(t_snake *snake)
{
	char	map[16][16];
	int		x_iter;
	int		y_iter;
	char	c;

	x_iter = 0;
	y_iter = 0;

	while (y_iter < 16)
	{
		while (x_iter < 16)
		{
			map[y_iter][x_iter] = '.';
			x_iter ++;
		}
		x_iter = 0;
		y_iter ++;
	}
	map[snake->apple[Y_POS]][snake->apple[X_POS]] = '#';

	
	x_iter = 0;
	while (x_iter < snake->snake_len && x_iter < 64)
	{
		if (snake->body[x_iter][Y_POS] >= 0 && snake->body[x_iter][Y_POS] < 16 && snake->body[x_iter][X_POS] >= 0 && snake->body[x_iter][X_POS] < 16)
		{
			if (x_iter == 0)
				c = 'O';
			else if (x_iter % 2)
				c = 'x';
			else
				c = 'X';
			map[snake->body[x_iter][Y_POS]][snake->body[x_iter][X_POS]] = c;
		}
		x_iter ++;
	}


	x_iter = 0;
	y_iter = 0;
	while (y_iter < 16)
	{
		while (x_iter < 16)
		{
			c = map[y_iter][x_iter];
			if (c == 'O')
				printf(GREEN);
			else if (c == 'x' || c == 'X')
				printf(YELLOW);
			else if (c == '#')
				printf(MAGENTA);
			printf("%c%s", c, RESET);
			x_iter ++;
		}
		x_iter = 0;
		y_iter ++;
		printf("\n");
	}
	printf(CURSOR_UP_16);
}

void	move_snake(t_snake *snake)
{
	int	snake_iterator;

	snake_iterator = snake->snake_len - 1;
	if (snake_iterator >= 64)
		snake_iterator = 63;
	while (snake_iterator > 0)
	{
		snake->body[snake_iterator][X_POS] = snake->body[snake_iterator - 1][X_POS];
		snake->body[snake_iterator][Y_POS] = snake->body[snake_iterator - 1][Y_POS];
		snake_iterator --;
	}
	if (snake->snake_direction == D_UP)
		snake->body[0][Y_POS] --;
	else if (snake->snake_direction == D_DOWN)
		snake->body[0][Y_POS] ++;
	else if (snake->snake_direction == D_LEFT)
		snake->body[0][X_POS] --;
	else if (snake->snake_direction == D_RIGHT)
		snake->body[0][X_POS] ++;
}

int	pseudo_random_number(t_snake *snake)
{
	static int number = 3;

	number += (long)snake % 18;
	number += snake->apple[X_POS] + snake->body[0][Y_POS] + 3;
	number *= 13;
	number = number % 16;
	return (number);
}

void	eat_apple(t_snake *snake)
{
	if (snake->snake_len < 64)
	{
		snake->body[snake->snake_len][Y_POS] = -1;
		snake->body[snake->snake_len][X_POS] = -1;
	}
	snake->snake_len ++;
	snake->apple[X_POS] = pseudo_random_number(snake);
	snake->apple[Y_POS] = pseudo_random_number(snake);
}

int	check_death(t_snake *snake)
{
	int	i;

	i = snake->snake_len - 1;
	while (i > 0)
	{
		if (snake->body[0][X_POS] == snake->body[i][X_POS])
			if (snake->body[0][Y_POS] == snake->body[i][Y_POS])
				return (1);
		i--;
	}
	if (snake->body[0][X_POS] >= 16 || snake->body[0][X_POS] < 0)
		return (1);
	if (snake->body[0][Y_POS] >= 16 || snake->body[0][Y_POS] < 0)
		return (1);
	return (0);
}

int	main(void)
{
	t_snake	*snake;
	unsigned long	wait_time;

	snake = init_snake();
	set_terminal_mode(1);
	wait_time = 200000000;

	while (1)
	{
		snake->snake_direction = get_head_direction(snake->snake_direction);
		move_snake(snake);
		if (snake->body[0][X_POS] == snake->apple[X_POS] && snake->body[0][Y_POS] == snake->apple[Y_POS])
			eat_apple(snake);
		print_map(snake);
		busy_wait(wait_time);
		wait_time -= wait_time / 250;
		if (check_death(snake))
			break ;
	}
	printf("You lost! :(     \n");
	printf("Your score: %s%i%s          \n", GREEN, snake->snake_len, RESET);
	set_terminal_mode(0);
	free(snake);
	return (0);
}
