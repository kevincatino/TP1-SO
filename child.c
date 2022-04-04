int main(int argc, char * argv[]) {

    int readfd = writePipes[i][0];
    int writefd = readPipes[i][1];
    int y;
    if (read(readfd, &y, sizeof(int)) == -1) { printf("Error at reading\n"); return 3; }
    printf("Procese archivo y recibi %d\n", y);
    printf("Envio valor %d a main\n", y);
    if (write(writefd,&y, sizeof(int)) == -1) { printf("Error at writing\n"); return 4; }


    // char *args[]={"/home/kevincatino/Arqui/tp0/p",NULL};
    // if(execvp(args[0],args) == -1) {
    //     perror("Error al ejecutar programa padre");
    // }
    return 0;
}