# 1 filepath.Clean() 

在 Go 语言中， *路径* 包用于由正斜杠分隔的路径，例如 URL 中的路径。 的 **filepath.Clean()** 函数用于通过纯词法处理返回与指定路径等效的最短路径名。 而且，这个函数是在path包下定义的。 在这里，您需要导入“path/filepath”包才能使用这些功能。 

此函数迭代地应用以下规则，直到无法进行进一步处理： 

- 它将多个分隔符元素替换为一个。 
- 如果指定路径为空字符串，则返回字符串“.”。 
- 它消除了每个 .  路径名元素（当前目录）。 
- 它消除了每个内部 .. 路径名元素（父目录）以及它之前的非 .. 元素。 
- 它消除了以根路径开头的 .. 元素：也就是说，假设 Separator 为“/”，将路径开头的“/..”替换为“/”。 

```
func Clean(path string) string
```

example

```go
// Golang program to illustrate the usage of
// filepath.Clean() function
  
// Including the main package
package main
  
// Importing fmt and path/filepath
import (
    "fmt"
    "path/filepath"
)
  
// Calling main
func main() {
  
    // Calling the Clean() function
    fmt.Println(filepath.Clean("/GFG/./../Geeks"))
    fmt.Println(filepath.Clean("GFG/../Geeks"))
    fmt.Println(filepath.Clean("..GFG/./../Geeks"))
    fmt.Println(filepath.Clean("gfg/../../../Geek/GFG"))
}
```

result

```go
/Geeks
Geeks
Geeks
../../Geek/GFG
```

**Example 2:**

```go
// Golang program to illustrate the usage of
// filepath.Clean() function
// Including the main package``package main
// Importing fmt and path/filepath
import (``  ``"fmt"``  ``"path/filepath"``)
// Calling main
func main() {
    // Calling the Clean() function
    fmt.Println(filepath.Clean(""))
    fmt.Println(filepath.Clean("."))
    fmt.Println(filepath.Clean("///"))
    fmt.Println(filepath.Clean("/.//"))
    fmt.Println(filepath.Clean("/./"))
    fmt.Println(filepath.Clean(":/"))
}
   
```

**Output:**

```
.
.
/
/
/
:
```