#include "Shader.h"
#include "Definitions.h"

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath); fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexPath << std::endl << infoLog << std::endl;
	};

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMEMT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setBool(const std::string& name, bool value)
{
	Shader::setInt(name, (int)value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float x, float y)
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setFloat(const std::string& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setFloat(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setTexture(std::shared_ptr<Texture> texture, int pos)
{
	// Get current shader program
	GLint currentProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

	// Set uniform
	glUseProgram(ID);
	setInt(texture->name, pos);

	// Restore shader program
	glUseProgram(currentProgramId);
}

void Shader::setMat4(const std::string& name, glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::prerender(std::shared_ptr<Camera> camera, std::shared_ptr<PointLight> light)
{
	GLint currentPorgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentPorgramId);

	use();
	setFloat("lightPos", light->getPosition().x, light->getPosition().y, light->getPosition().z);
	setFloat("lightColor", light->getColor().x, light->getColor().y, light->getColor().z);
	
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 projection = camera->GetProjectionMatrix();

	setMat4("view", view);
	setMat4("projection", projection);

	glm::vec3 viewPos = camera->GetPosition();
	setFloat("viewPos", viewPos.x, viewPos.y, viewPos.z);
	setFloat("fog_color", BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
	setFloat("water_fog_color", 0.f, .3f, .5f, 1.f);
	setFloat("inside_water", camera->GetPosition().y > 0 ? 0.f: 1.f);

	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 15.0f, -1.0f), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, .0f, 1.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	setMat4("light_space_matrix", lightSpaceMatrix);

	glUseProgram(currentPorgramId);
}

void Shader::lightSpaceTransform(std::shared_ptr<Light> light)
{
	GLint currentPorgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentPorgramId);

	glUseProgram(ID);
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 15.0f, .0f), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, .0f, 1.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	setMat4("light_space_matrix", lightSpaceMatrix);
	glUseProgram(currentPorgramId);
}