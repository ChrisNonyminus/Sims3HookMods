# Sims3HookMods

PROOF OF CONCEPT ASI for The Sims 3 (Steam version only right now, sorry) that hooks to the game's setup of its Mono/C# runtime, and adds custom internal calls, as well as searching for hypothetical DLL plugins that add their own internal calls.

Currently only adds one custom internal call: IO.File::CReadAllText. Its C++ prototype is ``bool FileReadAllText(const unsigned char* path, unsigned char* outText, size_t maxSize)``. Invoke it in your C# script in this way:

```cs
namespace IO
{
    public class File
    {
        [DllImport("Sims3Common")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool CReadAllText([In] byte[] filePath, [Out] byte[] text, int size);

        public static string ReadAllText(string filePath)
        {
            byte[] dest = new byte[1024];
           
            if (CReadAllText(System.Text.Encoding.UTF8.GetBytes(filePath), dest, 1024))
            {
                return System.Text.Encoding.UTF8.GetString(dest);
            }
            return "(null)";
        }
    }
}
```

## How to add custom internal calls.

Simply call ``MonoAddInternalCall(<function name>, <function pointer>)``. "function name" must be in this format: ``NamespaceName.Classname::FunctionName``. Which is invoked from C# such as:

```cs
namespace NamespaceName {
    public class ClassName {
        [DllImport("Sims3Common")]
        public static extern <function return type as marshalled by C#> FunctionName(<function arguments as marshalled by C#>);
    }
}
```