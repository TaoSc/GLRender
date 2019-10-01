#include "Renderer.h"

#include <math.h>
#include <array>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <assimp/version.h>
#include <GL/glew.h>
#include <SDL.h>
#include <SimpleIni.h>
#include <imgui.h>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"


Renderer::Renderer(std::string const& window_title, std::string const& directory, CSimpleIniA const& ini_file) :
	m_window_title(window_title), m_window_width(), m_window_height(),
	m_directory(directory), m_window(),
	m_context(), m_ini_file(ini_file), m_input()
{
	m_window_width = std::stoi(m_ini_file.GetValue("Video", "Width", "800"));
	m_window_height = std::stoi(m_ini_file.GetValue("Video", "Height", "600"));
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(m_context);
	SDL_Quit();
}

bool Renderer::init()
{
#ifdef GLM_VERSION
	std::cout << "Compiled against GLM version " << GLM_VERSION << "." << std::endl;
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, std::stoi(m_ini_file.GetValue("Video", "DoubleBuffer", "1")));
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	SDL_version compiled, linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("Compiled against SDL version %d.%d.%d.\n", compiled.major, compiled.minor, compiled.patch);
	printf("Linked against SDL version %d.%d.%d.\n", linked.major, linked.minor, linked.patch);


	m_window.reset(SDL_CreateWindow(m_window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_window_width, m_window_height, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL));
	if (m_window == 0)
	{
		std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}


	m_context = SDL_GL_CreateContext(m_window.get());
	if (m_context == 0)
	{
		std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
		m_window.reset();
		SDL_Quit();
		return false;
	}
	SDL_GL_SetSwapInterval(1);


	GLenum GLEW_initialization(glewInit());
	if (GLEW_initialization != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(GLEW_initialization) << std::endl;
		delete this;
		return false;
	}
	std::cout << "GLEW version " << glewGetString(GLEW_VERSION) << "." << std::endl;


	std::cout << "Built against Assimp version " << aiGetVersionMajor() << "." << aiGetVersionMinor() << "." << std::endl << std::endl;


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	return true;
}


void Renderer::mainLoop()
{
	constexpr Uint32 frame_rate(1000 / 60);
	Uint32 frame_start(0), elapsed_time(0), delta_time(0), last_frame(0);
	const float ratio(static_cast<float>(m_window_width) / static_cast<float>(m_window_height));
	GLfloat viewport_fov(70.0f);

	m_input.showCursor(false);
	m_input.catchCursor(true);


	// Matrices work
	glm::mat4 model, view, projection;
	projection = glm::perspective(glm::radians(viewport_fov), ratio, 0.1f, 100.0f);
	bool update_projection(false);


	// Camera work
	Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 3.75f, 1.1f };
	const std::map<std::string, SDL_Scancode> keys{
		{"forward",	 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "forward", "26")))},
		{"backward", static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "backward", "22")))},
		{"left",	 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "left", "4")))},
		{"right",	 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "right", "7")))},
		{"run",		 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "run", "225")))},
		{"crouch",	 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "crouch", "224")))},
		{"jump",	 static_cast<SDL_Scancode>(std::stoi(m_ini_file.GetValue("KeyboardMap", "jump", "44")))}
	};
	bool player_running(false);


	// Shaders loading
	Shader basic_shader{ m_directory + "Shaders/basic.vert", m_directory + "Shaders/basic.frag" },
		stencil_shader{ m_directory + "Shaders/stencil_outline.vert", m_directory + "Shaders/stencil_outline.frag" };
	glUseProgram(stencil_shader.id());
	stencil_shader.setUni("offset", 0.07f);
	stencil_shader.setUni("projection", projection);

	glUseProgram(basic_shader.id());
	basic_shader.setUni("material.shininess", 32.0f);
	basic_shader.setUni("projection", projection);


	// Lights setup
	basic_shader.setUni("lights[0].type", 0);
	basic_shader.setUni("lights[0].direction", -0.2f, -1.0f, -0.3f);
	basic_shader.setUni("lights[0].ambient", 0.2f, 0.2f, 0.2f);
	basic_shader.setUni("lights[0].diffuse", 0.5f, 0.5f, 0.5f);
	basic_shader.setUni("lights[0].specular", 1.0f, 1.0f, 1.0f);

	glm::vec3 point_lights_pos[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};
	size_t i = 1;
	for (glm::vec3 const& light_pos : point_lights_pos) {
		basic_shader.setUni("lights[" + std::to_string(i) + "].type", 1);
		basic_shader.setUni("lights[" + std::to_string(i) + "].position", light_pos);
		basic_shader.setUni("lights[" + std::to_string(i) + "].ambient", 0.2f, 0.2f, 0.2f);
		basic_shader.setUni("lights[" + std::to_string(i) + "].diffuse", 0.5f, 0.5f, 0.5f);
		basic_shader.setUni("lights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		basic_shader.setUni("lights[" + std::to_string(i) + "].constant", 1.0f);
		basic_shader.setUni("lights[" + std::to_string(i) + "].linear", 0.09f);
		basic_shader.setUni("lights[" + std::to_string(i) + "].quadratic", 0.032f);

		i++;
	}

	basic_shader.setUni("lights[5].type", 2);
	basic_shader.setUni("lights[5].cutoff", glm::cos(glm::radians(12.5f)));
	basic_shader.setUni("lights[5].outer_cutoff", glm::cos(glm::radians(17.5f)));
	basic_shader.setUni("lights[5].ambient", 0.2f, 0.2f, 0.2f);
	basic_shader.setUni("lights[5].diffuse", 0.5f, 0.5f, 0.5f);
	basic_shader.setUni("lights[5].specular", 1.0f, 1.0f, 1.0f);
	basic_shader.setUni("lights[5].constant", 1.0f);
	basic_shader.setUni("lights[5].linear", 0.09f);
	basic_shader.setUni("lights[5].quadratic", 0.032f);


	Shader lamp_shader{ m_directory + "Shaders/lamp.vert", m_directory + "Shaders/lamp.frag" };
	glUseProgram(lamp_shader.id());
	lamp_shader.setUni("projection", projection);

	glUseProgram(0);


	// Models loading
	Model cube{ m_directory + "Models/cube/cube.obj" };
	Model nanosuit{ m_directory + "Models/nanosuit/nanosuit.obj", GL_REPEAT };
	Model blades{ m_directory + "Models/blades/blades.obj", GL_CLAMP_TO_EDGE };
	Model window{ m_directory + "Models/transparent_window/transparent_window.obj", GL_CLAMP_TO_EDGE };
	const std::vector<glm::vec3> objects = { glm::vec3(0, 1.f, -2.f), glm::vec3(0, 0.f, -3.f) };


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, m_window_width, m_window_height);




	// Main loop
	while (!m_input.end())
	{
		frame_start = SDL_GetTicks();
		delta_time = frame_start - last_frame;
		last_frame = frame_start;

		m_input.updateEvents();
		if (m_input.isKeyPressed(SDL_SCANCODE_ESCAPE))
			break;

		if (m_input.hasWheelMoved() && ((m_input.getWheelY() < 0 && viewport_fov < 100.0f) || (m_input.getWheelY() > 0 && viewport_fov > 30.0f)))
		{
			viewport_fov -= m_input.getWheelY() * 5;
			update_projection = true;
		}
		else
			update_projection = false;
		if (m_input.isKeyPressed(keys.at("run")) && !player_running) {
			camera.setSpeed(camera.getSpeed() * 2);
			player_running = true;
			viewport_fov += 10;
			update_projection = true;
		}
		if (m_input.isKeyReleased(keys.at("run")) && player_running) {
			camera.setSpeed(camera.getSpeed() / 2);
			player_running = false;
			viewport_fov -= 10;
			update_projection = true;
		}
		if (update_projection)
			projection = glm::perspective(glm::radians(viewport_fov), ratio, 0.1f, 100.0f);

		camera.shift(m_input, keys, delta_time / 100.0f);
		view = camera.lookAt(); // Should the camera.lookAt() return value be passed directly to the view uniform, removing the need for the view var


		glClearColor(.125f, .25f, .25f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		// Fancy work starts here:
		glDisable(GL_BLEND);
		glStencilMask(0x00);
		glUseProgram(lamp_shader.id());

		if (update_projection)
			lamp_shader.setUni("projection", projection);
		lamp_shader.setUni("view", view);

		for (glm::vec3 const& light_pos : point_lights_pos) {
			model = glm::translate(glm::mat4(1.f), light_pos);
			model = glm::scale(model, glm::vec3(0.2f));
			lamp_shader.setUni("model", model);

			cube.Draw(lamp_shader);
		}

		// Distance sorting
		std::map<float, glm::vec3> sorted;
		for (size_t i = 0; i < objects.size(); i++)
		{
			float distance = glm::length(camera.getPosition() - objects[i]);
			sorted[distance] = objects[i];
		}


		glUseProgram(basic_shader.id());

		if (update_projection)
			basic_shader.setUni("projection", projection);
		basic_shader.setUni("view", view);
		basic_shader.setUni("view_pos", camera.getPosition());
		basic_shader.setUni("lights[5].position", camera.getPosition());
		basic_shader.setUni("lights[5].direction", camera.getOrientation());

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		model = glm::translate(glm::mat4(1.f), glm::vec3(0, -10.f, -5.f));
		basic_shader.setUni("model", model);
		nanosuit.Draw(basic_shader);

		glStencilMask(0x00);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::translate(glm::mat4(), it->second);
			basic_shader.setUni("model", model);

			if (it->second == glm::vec3(0, 0.f, -3.f))
				blades.Draw(basic_shader);
			else
				window.Draw(basic_shader);
		}


		glDisable(GL_BLEND);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);
		glUseProgram(stencil_shader.id());

		if (update_projection)
			stencil_shader.setUni("projection", projection);
		model = glm::translate(glm::mat4(1.f), glm::vec3(0, -10.f, -5.f));
		stencil_shader.setUni("view", view);
		stencil_shader.setUni("model", model);
		nanosuit.Draw(stencil_shader, false);

		glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glUseProgram(0);

		SDL_GL_SwapWindow(m_window.get());

		elapsed_time = SDL_GetTicks() - frame_start;
		if (elapsed_time < frame_rate)
			SDL_Delay(frame_rate - elapsed_time);
	}
}
