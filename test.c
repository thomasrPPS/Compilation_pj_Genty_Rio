// Un exemple de programme MiniC
int start = 0;
int end = 5;
void main () {
	int i, s = start , e = end ;
	int sum = 0;
	for (i = s; i < e; i = i + 1) {
 		sum = sum + 1;	
 	}
	print("sum :", sum , "\n") ;
}