//RIO le 28/03

bool retour = true;

int a=0;
int b=0;

void main() {
    a = 10;
    b = 2;

    if(a != 10){
        if(b != 2){

        }
    }
    if((a/b) != 5){
        retour = false;
    }
    else if((a*b) != 20) {
        retour = false;
    }
    else if((a+b) != 12){
        retour = false;
    }
    else if((a-b) != 8){
        retour = false;
    }
    else if(!retour){
        retour = false;
    }
    else if(~a==1){
        retour = false;
    }
    else if(-a == -2){
        retour = false;
    }
    else if(a || 0){
        retour = false;
    }
    else if((retour && 1)==0){
        retour = false;
    }
    else if(a != a){
        retour = false;
    }
    else if(a == b){
        retour = false;
    }
    else if(0){
        retour = false;
    }
    else if(!(a || a)){
        retour = false;
    }
    else if(!(a & a)){
        retour = false;
    }
    else if(a<=9 || !(a<=10)){
        retour = false;
    }
    else if(a>=11 || !(a>=10)){
        retour = false;
    }
    else if(a<b){
        retour = false;
    }
    else if(b>a){
        retour = false;
    }
    else if((a>>(b))-2){
        retour = false;
    }
    else if(0){
        retour = false;
    }
    else if(a<<(b+7) != 5120){
        retour = false;
    }
    else if((a%(b+7))!= 1){
        retour = false;
    }
    if(retour == true){
        print("SUCCESS");
    }
    else {
        print("ERROR");
    }
}
