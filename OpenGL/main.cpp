#define GL_SILENCE_DEPRECATION ; //用于取消deprecated警告
#include <GL/glew.h> //要放在最前面
#include <GLFW/glfw3.h>
#include <iostream>


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    //这是编译一个着色器需要写的所有代码
    
    //指明要生成的shader类型
    unsigned int id = glCreateShader(type);
    
    //返回一个以null结尾的不可变数组指针, 指向数组的第一个元素的地址, src就是一个raw string(原生的纯string)
    const char* src = source.c_str();// == &source[0]
    
    /*
    glShaderSource — Replaces the source code in a shader object 替换shader对象的source code
        shader
            Specifies the handle of the shader object whose source code is to be replaced. 指明被替换源码的shader对象的handle
        count
            Specifies the number of elements in the string and length arrays. 指明数组中source code的数量
        string
            Specifies an array of pointers to strings containing the source code to be loaded into the shader. 指明指向source code字符串的指针数组
        length
            Specifies an array of string lengths. 指明一个字符串长度数组
     */
    glShaderSource(id, 1, &src, nullptr);
    
    //Compiles the source code strings that have been stored in the shader object specified by shader.
    glCompileShader(id);
    
    //Error handling
    int result;
    
    /*
    glGetShaderiv - returns in params the value of a parameter for a specific shader object.
        GL_SHADER_TYPE
            GL_VERTEX_SHADER if shader is a vertex shader object,
            GL_GEOMETRY_SHADER if shader is a geometry shader object, and
            GL_FRAGMENT_SHADER if shader is a fragment shader object.
        GL_DELETE_STATUS
            GL_TRUE if shader is currently flagged for deletion, and
            GL_FALSE otherwise.
        GL_COMPILE_STATUS
            GL_TRUE if the last compile operation on shader was successful, and
            GL_FALSE otherwise.
        GL_INFO_LOG_LENGTH
            the size of the character buffer required to store the information log including the null termination character
            If shader has no information log, a value of 0 is returned.
        GL_SHADER_SOURCE_LENGTH
            the size of the character buffer required to store the shader source, including the null termination character.
            If no source code exists, 0 is returned.
     */
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        
        /*
        shader
            Specifies the shader object whose information log is to be queried.
        maxLength
            Specifies the size of the character buffer for storing the returned information log.
        length
            Returns the length of the string returned in infoLog (excluding the null terminator).
        infoLog
            Specifies an array of characters that is used to return the information log.
        */
        glGetShaderInfoLog(id, length, &length, message);
        
        std::cout << "Failed to compile" <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(0);
    }
    
    //返回id,赋值给vs
    return id;
}

    // 这个函数的参数是传入vertexshader和fragmentshader的source code
static unsigned int CreateShader(const std::string& vertexShader, std::string& fragmentShader)
{
    
    /*
    creates an empty program object and returns a non-zero value by which it can be referenced
    A program object is an object to which shader objects can be attached.
    This provides a mechanism to specify the shader objects that will be linked to create a program.
    It also provides a means for checking the compatibility of the shaders that will be used to create a program
    */
    unsigned int program = glCreateProgram();
    unsigned int vs =  CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs =  CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    /*
     Attaches a shader object to a program object
        program
            Specifies the program object to which a shader object will be attached.
        shader
            Specifies the shader object that is to be attached.
     */
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    //Links a program object
    glLinkProgram(program);
    
    /*Validates a program object
    checks to see whether the executables contained in program can execute given the current OpenGL state.
    The information generated by the validation process will be stored in program's information log.*/
    glValidateProgram(program);
    
    //因为shader已经被link到program中了
    //所以可以delete了
    //不用detach是因为detach删除源码, 不利于后期debug
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //这个函数需在一个opengl rendering context被渲染之后才能被call,也就是上面这个glfwMakeContextCurrent
    if(glewInit() != GLEW_OK)
        std::cout << "Error" << std::endl;
    
    //某一个新OpenGL函数
    //unsigned int a;
    //glGenBuffers(1, &a);
    
    //vertex位置
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    
    /*give OpenGL the data
    generate一个buffer,给我们一个ID;用于储存buffer的地址
    选择使用的buffer
    在buffer中存入position(六个浮点数大小的数组)*/
    unsigned int buffer;
    
    /*
    Specifies the number of buffer object names to be generated.
    Specifies an array in which the generated buffer object names are stored.
    Buffer object names are unsigned integers.
    The value zero is reserved, but there is no default buffer object for each buffer object target.
     */
    glGenBuffers(1, &buffer);//generate一个buffer,给我们一个ID;用于储存buffer的地址
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//选择使用的buffer
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    
    //enable vertex启动vertex
    glEnableVertexAttribArray(0);
    
    /*
     vertex属性指针,用于指定一个buffer的布局
    因为positions只有一个属性,所以只需要调用一次这个函数
    gen的第几个buffer,第一个所以是0
    这个属性有两个components所以写2
    positions中全是float所以用GL_FLOAT
    是否选择初始化,这里因为position中已经都是float了,所以就不用初始化了
    vertex中该属性的大小,两个float所以是 4byte * 2 = 8
    到达下一个属性需要的byte,因为我们只有一个属性所以就是0,不然的话就是传入(const int*)
     */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0,0.0, 1.0);\n"
        "}\n";
    
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //两种methods来画一个三角形
        glDrawArrays(GL_TRIANGLES, 0, 3);//从第几个点开始,画几个点
        //glDrawElements(GL_TRIANGLES, 3, unsigned int, NULL)//几个点,什么类型的输入,还没讲

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
