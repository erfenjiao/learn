一级标题：　<h1>标题</h1> 

段落：　　　<p> </p>

网页中所有内容都写在这里面 : <html></html>

网页中的子标签: 　　　　<html><head></head><body></body></html>

网页中的主体元素都在:     <body></body>

自结束标签：<input />

注释：　　　<!-- -->

标签中的属性,在开始标签中设置；属性是一个名值对结构(x=y)

图像标签：<img src = "图像文件地址">

```html
<img src = "baidu.gif" width = 174 height = 59 hspace = 5 vapace = 5 border = 0
     align = "top" alt = "说明文字" lowsrc = "prelogo.gif">AAA
</img>
```

HTML元素分为块级元素或内行元素。

块级元素多为结构性标记，以新的一行作为开始，如<h1> <center> <table> <form>

> 总是从新的一行作为开始
>
> 高度、宽度都是可控的
>
> 宽度没有设置时，默认为100%
>
> 块级元素中可以包含块级元素和行内元素

内联元素多为描述性标记，如<a> <b> <img>

> 和其他元素都在一行

区块元素(DIV SPAN):

DIV元素用于设定页面布局，以及文字、图片、表格等标记的摆放位置

SPAN元素是内联元素，本身没有特定的含义，常用作文本的容器

DIV SPAN通过CSS式样式表增强显示能力