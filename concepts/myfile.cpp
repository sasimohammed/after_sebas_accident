// ID1: 20231079 - sohila walid mohammed
// ID2: 20230294 - klara sameh
// ID3: 20231087 - seba ibraheem
// ID4: 20230289 - katren nader

#include <iostream>
#include <stack>
using namespace std;


struct Frame
{
    int n;
    int pc;
    int a;
    int b;
    int c;
};

int F(int n)
{
    stack<Frame> st;
    int retVal = 0;

    Frame f0;
    f0.n = n;
    f0.pc = 0;
    f0.a = 0;
    f0.b = 0;
    f0.c = 0;
    st.push(f0);

    while (!st.empty())
    {
        Frame &cur = st.top();

        if (cur.pc == 0)
        {
            if (cur.n <= 1)
            {
                retVal = 1;
                st.pop();
            }
            else
            {
                cur.pc = 1;
                Frame nf;
                nf.n = cur.n - 2;
                nf.pc = 0;
                nf.a = 0;
                nf.b = 0;
                nf.c = 0;
                st.push(nf);
            }
        }
        else if (cur.pc == 1)
        {
            cur.a = cur.n + retVal;
            cur.pc = 2;
            Frame nf;
            nf.n = cur.n / 2;
            nf.pc = 0;
            nf.a = 0;
            nf.b = 0;
            nf.c = 0;
            st.push(nf);
        }
        else if (cur.pc == 2)
        {
            cur.b = (cur.n - 1) * retVal;
            cur.c = cur.n - 1 - (cur.a + cur.b) % 3;
            cur.pc = 3;
            Frame nf;
            nf.n = cur.c;
            nf.pc = 0;
            nf.a = 0;
            nf.b = 0;
            nf.c = 0;
            st.push(nf);
        }
        else
        {
            int d = retVal;
            retVal = cur.a + cur.b + d;
            st.pop();
        }
    }

    return retVal;
}

int main()
{
    int i;
    for (i = 1; i <= 20; i++)
    {
        cout << "F(" << i << ") = " << F(i) << "\n";
    }
    return 0;
}