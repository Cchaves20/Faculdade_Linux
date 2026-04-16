int boba(float v[], float lim, int n) 
{
    int i;
    int a = 0;
    for(i = 0; i < n; i++) 
    {
        if(v[i] > lim) a += 1;
    }
    return a;
} 