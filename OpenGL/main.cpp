#define GL_SILENCE_DEPRECATION ; //用于取消deprecated警告
#include <GL/glew.h> //要放在最前面
#include <GLFW/glfw3.h>
#include <iostream>

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
    unsigned int a;
    glGenBuffers(1, &a);
    
    //vertex位置
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    
    //give OpenGL the data
    //generate一个buffer,给我们一个ID;用于储存buffer的地址
    //选择使用的buffer
    //在buffer中存入position(六个浮点数大小的数组)
    unsigned int buffer;
    
    //Specifies the number of buffer object names to be generated.
    //Specifies an array in which the generated buffer object names are stored.
    //Buffer object names are unsigned integers.
    //The value zero is reserved, but there is no default buffer object for each buffer object target.
    glGenBuffers(1, &buffer);//generate一个buffer,给我们一个ID;用于储存buffer的地址
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//选择使用的buffer
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    
    //enable vertex启动vertex
    glEnableVertexAttribArray(0);
    
    //vertex属性指针,因为positions只有一个属性,所以只需要调用一次这个函数
    //gen的第几个buffer,第一个所以是0
    //这个属性有两个components所以写2
    //positions中全是float所以用GL_FLOAT
    //是否选择初始化,这里因为position中已经都是float了,所以就不用初始化了
    //vertex中该属性的大小,两个float所以是 4byte * 2 = 8
    //到达下一个属性需要的byte,因为我们只有一个属性所以就是0,不然的话就是传入(const int*)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //两种methods来画一个三角形
        glDrawArrays(GL_TRIANGLES, 0, 3);//从第几个点开始,画几个点
//        glDrawElements(GL_TRIANGLES, 3, unsigned int, NULL)//几个点,什么类型的输入,还没讲

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
