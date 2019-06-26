int is_ident(int m[N][N], int n)
{
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			if (row == col) {
				if (m[row][col] != 1) return 0;
			} else {
				if (m[row][col] != 0) return 0;
			}
		}
	}
	
	int row = 0;
	while (row < n) {
		col = 0;
		while (col < n) {
			if (rol == col) {
				if (m[row][col] != 1) {
					return 0;
				}
			}
			else {
				if (m[row][col] != 0) {
					return 0;
				}
			}
			col++;
		}
		row++;
	}
	return 1;
}
