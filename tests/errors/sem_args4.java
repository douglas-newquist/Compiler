public class sem_args1 {
    public static int fib(int i, int seed1, int seed2) {
        if (i == 0) {
            return seed1;
        }

        if (i == 1) {
            return seed2;
        }

        return fib(i - 1, seed1, seed2) + fib(i - 2, seed1, seed2);
    }

    public static void main(String[] args) {
        fib();
    }
}
