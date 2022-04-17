// Un exemple de programme MiniC
int start = 0;
int end = 2147483648;
bool x, y = true;
//bool start;

void main () {
	int i, s = start , e = end ;
	int sum = 0;
	for (i = s; i < e; i = i + 1) {
		sum = sum + i;	
 	}
	print ("sum: ", sum , "\n") ;

}