using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;

namespace test01csharp
{
    class testDll
    {

        [DllImport("mri_opendds", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int getX();



        public void getY()
        {
            int xjdhh = 101;
            xjdhh--;

        }


        
    }
}
