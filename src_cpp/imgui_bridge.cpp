#include "imgui_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"


static void check_vk_result(VkResult err)
{
  if (err == VK_SUCCESS)
    return;

  fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", err);

  if (err < 0)
    exit(EXIT_FAILURE);
}

void imgui_apply_style()
{
  ImGuiStyle& style = ImGui::GetStyle();

  // ------------------------------------------------------------
  // Geometry
  // ------------------------------------------------------------

  style.WindowRounding    = 7.0f;
  style.ChildRounding     = 6.0f;
  style.FrameRounding     = 5.0f;
  style.PopupRounding     = 6.0f;
  style.ScrollbarRounding = 5.0f;
  style.GrabRounding      = 5.0f;

  style.WindowBorderSize  = 1.0f;
  style.ChildBorderSize   = 1.0f;
  style.PopupBorderSize   = 1.0f;
  style.FrameBorderSize   = 0.0f;

  style.FramePadding = ImVec2(12.0f, 6.0f);
  style.ItemSpacing  = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

  style.WindowPadding = ImVec2(12.0f, 10.0f);

  style.ScrollbarSize = 12.0f;
  style.GrabMinSize   = 10.0f;

  style.SeparatorTextBorderSize = 1.0f;

  // Slightly larger UI
  ImGui::GetIO().FontGlobalScale = 1.05f;


  // ------------------------------------------------------------
  // Colors
  // ------------------------------------------------------------

  ImVec4* colors = style.Colors;

  // Main background
  colors[ImGuiCol_WindowBg] =
    ImVec4(0.055f, 0.065f, 0.085f, 1.00f);

  colors[ImGuiCol_ChildBg] =
    ImVec4(0.060f, 0.070f, 0.090f, 1.00f);

  colors[ImGuiCol_PopupBg] =
    ImVec4(0.075f, 0.085f, 0.105f, 0.98f);

  // Top menu bar
  colors[ImGuiCol_MenuBarBg] =
    ImVec4(0.085f, 0.095f, 0.120f, 1.00f);

  // Borders
  colors[ImGuiCol_Border] =
    ImVec4(0.20f, 0.25f, 0.30f, 0.65f);

  colors[ImGuiCol_BorderShadow] =
    ImVec4(0.0f, 0.0f, 0.0f, 0.20f);


  // ------------------------------------------------------------
  // Text
  // ------------------------------------------------------------

  colors[ImGuiCol_Text] =
    ImVec4(0.91f, 0.93f, 0.95f, 1.00f);

  colors[ImGuiCol_TextDisabled] =
    ImVec4(0.45f, 0.49f, 0.54f, 1.00f);


  // ------------------------------------------------------------
  // Menu interaction
  // ------------------------------------------------------------

  colors[ImGuiCol_Header] =
    ImVec4(0.11f, 0.13f, 0.16f, 1.00f);

  colors[ImGuiCol_HeaderHovered] =
    ImVec4(0.08f, 0.43f, 0.50f, 0.65f);

  colors[ImGuiCol_HeaderActive] =
    ImVec4(0.06f, 0.55f, 0.63f, 0.85f);


  // ------------------------------------------------------------
  // Buttons
  // ------------------------------------------------------------

  colors[ImGuiCol_Button] =
    ImVec4(0.10f, 0.12f, 0.15f, 1.00f);

  colors[ImGuiCol_ButtonHovered] =
    ImVec4(0.10f, 0.30f, 0.34f, 1.00f);

  colors[ImGuiCol_ButtonActive] =
    ImVec4(0.06f, 0.55f, 0.63f, 1.00f);


  // ------------------------------------------------------------
  // Separators
  // ------------------------------------------------------------

  colors[ImGuiCol_Separator] =
    ImVec4(0.25f, 0.32f, 0.36f, 0.55f);

  colors[ImGuiCol_SeparatorHovered] =
    ImVec4(0.10f, 0.55f, 0.63f, 0.75f);

  colors[ImGuiCol_SeparatorActive] =
    ImVec4(0.10f, 0.65f, 0.73f, 1.00f);


  // ------------------------------------------------------------
  // Scrollbars
  // ------------------------------------------------------------

  colors[ImGuiCol_ScrollbarBg] =
    ImVec4(0.04f, 0.05f, 0.065f, 1.00f);

  colors[ImGuiCol_ScrollbarGrab] =
    ImVec4(0.18f, 0.21f, 0.25f, 1.00f);

  colors[ImGuiCol_ScrollbarGrabHovered] =
    ImVec4(0.25f, 0.30f, 0.34f, 1.00f);

  colors[ImGuiCol_ScrollbarGrabActive] =
    ImVec4(0.10f, 0.55f, 0.63f, 1.00f);
}

void imgui_apply_style2()
{
  ImGuiStyle& style = ImGui::GetStyle();

  style.WindowRounding    = 8.0f;
  style.FrameRounding     = 6.0f;
  style.PopupRounding     = 6.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabRounding      = 6.0f;

  style.FramePadding   = ImVec2(16.0f, 8.0f);
  style.ItemSpacing     = ImVec2(12.0f, 6.0f);
  style.WindowPadding   = ImVec2(12.0f, 10.0f);
  style.SeparatorTextBorderSize = 1.0f;

  ImGui::GetIO().FontGlobalScale = 1.05f;

  ImVec4* colors = style.Colors;

  // Base — deep slate, not pure black/gray
  colors[ImGuiCol_MenuBarBg]      = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
  colors[ImGuiCol_WindowBg]       = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
  colors[ImGuiCol_PopupBg]        = ImVec4(0.11f, 0.12f, 0.15f, 0.98f);
  colors[ImGuiCol_Border]         = ImVec4(0.20f, 0.45f, 0.50f, 0.35f); // faint teal edge, not gray

  // Buttons — neutral by default, accent only on interaction
  colors[ImGuiCol_Button]         = ImVec4(0.15f, 0.16f, 0.20f, 1.00f);
  colors[ImGuiCol_ButtonHovered]  = ImVec4(0.16f, 0.55f, 0.60f, 0.55f); // teal, not blue
  colors[ImGuiCol_ButtonActive]   = ImVec4(0.13f, 0.68f, 0.75f, 1.00f);

  // Menu items (File/Edit dropdowns)
  colors[ImGuiCol_HeaderHovered]  = ImVec4(0.13f, 0.68f, 0.75f, 0.35f);
  colors[ImGuiCol_HeaderActive]   = ImVec4(0.13f, 0.68f, 0.75f, 0.55f);

  // Text
  colors[ImGuiCol_Text]           = ImVec4(0.90f, 0.92f, 0.93f, 1.00f);
  colors[ImGuiCol_TextDisabled]   = ImVec4(0.45f, 0.48f, 0.52f, 1.00f);

  // Separator
  colors[ImGuiCol_Separator]      = ImVec4(0.20f, 0.45f, 0.50f, 0.40f);
}

void imgui_init(Renderer* renderer, Window* window)
{
    QueueFamilyIndices indices =
        findQueueFamilies(renderer->surface, renderer->deviceCtx.physicalDevice);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Inter_18pt-Regular.ttf", 16.0f);
    ImGui::StyleColorsDark();
    imgui_apply_style();

    ImGui_ImplGlfw_InitForVulkan(window->window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};

    init_info.Instance =
        renderer->instanceCtx.instance;

    init_info.PhysicalDevice =
        renderer->deviceCtx.physicalDevice;

    init_info.Device =
        renderer->deviceCtx.device;

    init_info.QueueFamily =
        indices.graphicsFamily;

    init_info.Queue =
        renderer->deviceCtx.graphicsQueue;

    init_info.PipelineCache =
        VK_NULL_HANDLE;

    init_info.DescriptorPool =
        renderer->descriptorsCtx.imguiPool;

    init_info.Allocator =
        NULL;

    init_info.MinImageCount =
        renderer->swapChainCtx.imageCount;

    init_info.ImageCount =
        renderer->swapChainCtx.imageCount;

    init_info.PipelineInfoMain.RenderPass =
        renderer->pipelineCtx.renderPass;

    init_info.PipelineInfoMain.Subpass =
        0;

    init_info.PipelineInfoMain.MSAASamples =
        VK_SAMPLE_COUNT_4_BIT;

    init_info.CheckVkResultFn =
        check_vk_result;

    ImGui_ImplVulkan_Init(&init_info);

    // One-time font upload
    //ImGui_ImplVulkan_CreateFontsTexture();
}

void imgui_destroy(){
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

  
void imgui_new_frame(){
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();   // <- was just NewFrame()
}

void updateDiagram(World* world,Renderer* renderer,FeaModel* model,DIAGRAM_TYPE type){
  VkDeviceSize meshUniformBufferSize = sizeof(UniformBufferMesh);
  if(renderer->show_diagram){
    vkDeviceWaitIdle(renderer->deviceCtx.device);
    vkDestroyBuffer((renderer->deviceCtx.device), renderer->diagramBuffer.vertexBuffer, NULL);
    vkFreeMemory(renderer->deviceCtx.device, renderer->diagramBuffer.vertexBufferMemory, NULL);
    for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      vkUnmapMemory(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffersMemory[j]);

      vkDestroyBuffer(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffers[j], NULL);
      vkFreeMemory(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffersMemory[j], NULL);
    }
    if(renderer->diagram.vertices){
      free(renderer->diagram.vertices);
      renderer->diagram.vertexCount = 0;
    }
  }else{
    renderer->show_diagram = 1;
  }
  createBeamDiagram(model,renderer,&(renderer->diagram),type);
  createDiagramBuffer(&(renderer->diagramBuffer),&(renderer->deviceCtx),&(renderer->commandsCtx),&renderer->diagram);
  createUniformBuffers(&(renderer->diagramUniform),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
  for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
    uint32_t diagramSet =
      (renderer->meshBuffersCount + world->gridCount) * MAX_FRAMES_IN_FLIGHT + j;

    VkDescriptorBufferInfo bufferInfoGlobal = {0};
    bufferInfoGlobal.buffer = renderer->globalUniform.uniformBuffers[j];
    bufferInfoGlobal.offset = 0;
    bufferInfoGlobal.range = sizeof(UniformBufferGlobal);

    VkWriteDescriptorSet writeGlobal = {0};
    writeGlobal.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeGlobal.dstSet = renderer->descriptorsCtx.sets[diagramSet];
    writeGlobal.dstBinding = 0;
    writeGlobal.dstArrayElement = 0;
    writeGlobal.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeGlobal.descriptorCount = 1;
    writeGlobal.pBufferInfo = &bufferInfoGlobal;
    vkUpdateDescriptorSets(renderer->deviceCtx.device, 1, &writeGlobal, 0, NULL);

    VkDescriptorBufferInfo bufferInfoDiagram = {0};
    bufferInfoDiagram.buffer = renderer->diagramUniform.uniformBuffers[j];
    bufferInfoDiagram.offset = 0;
    bufferInfoDiagram.range = meshUniformBufferSize;

    VkWriteDescriptorSet writeDiagram = {0};
    writeDiagram.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDiagram.dstSet = renderer->descriptorsCtx.sets[diagramSet];
    writeDiagram.dstBinding = 1;
    writeDiagram.dstArrayElement = 0;
    writeDiagram.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDiagram.descriptorCount = 1;
    writeDiagram.pBufferInfo = &bufferInfoDiagram;
    vkUpdateDescriptorSets(renderer->deviceCtx.device, 1, &writeDiagram, 0, NULL);
  }
}

void inspectionFunction(World* world,FeaModel* model);

void imgui_draw(World* world,Renderer* renderer,FeaModel* model)
{
  uint32_t old_flags = world->render_flags;
  if (ImGui::BeginMainMenuBar())
    {
      // ========================================================
      // Subtle accent line underneath the menu bar
      // ========================================================

      ImDrawList* drawList = ImGui::GetWindowDrawList();

      ImVec2 barPos  = ImGui::GetWindowPos();
      ImVec2 barSize = ImGui::GetWindowSize();

      drawList->AddRectFilled(
			      ImVec2(barPos.x, barPos.y + barSize.y - 2.0f),
			      ImVec2(barPos.x + barSize.x, barPos.y + barSize.y),
			      IM_COL32(20, 170, 190, 220)
			      );


      // ========================================================
      // File
      // ========================================================

      if (ImGui::BeginMenu("File"))
        {
	  if (ImGui::MenuItem("New", "Ctrl+N")) {}
	  if (ImGui::MenuItem("Open...", "Ctrl+O")) {}

	  ImGui::Separator();

	  if (ImGui::MenuItem("Save", "Ctrl+S")) {}
	  if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}

	  ImGui::Separator();

	  if (ImGui::MenuItem("Exit", "Alt+F4")) {}

	  ImGui::EndMenu();
        }


      // ========================================================
      // Edit
      // ========================================================

      if (ImGui::BeginMenu("Edit"))
        {
	  if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
	  if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}

	  ImGui::EndMenu();
        }


      // ========================================================
      // Separator
      // ========================================================

      ImGui::SameLine(0.0f, 18.0f);
      ImGui::Separator();
      ImGui::SameLine(0.0f, 18.0f);


      // ========================================================
      // Solve
      // ========================================================

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

      ImGui::PushStyleColor(
			    ImGuiCol_Button,
			    ImVec4(0.06f, 0.50f, 0.60f, 1.00f)
			    );

      ImGui::PushStyleColor(
			    ImGuiCol_ButtonHovered,
			    ImVec4(0.08f, 0.62f, 0.72f, 1.00f)
			    );

      ImGui::PushStyleColor(
			    ImGuiCol_ButtonActive,
			    ImVec4(0.04f, 0.42f, 0.50f, 1.00f)
			    );

      if (ImGui::Button("Solve"))
        {
	  // solve
	  beamSolver(model);
	  model->fea_solved = 1;
        }

      ImGui::PopStyleColor(3);
      ImGui::PopStyleVar();


      // ========================================================
      // Separator
      // ========================================================

      ImGui::SameLine(0.0f, 18.0f);
      ImGui::Separator();
      ImGui::SameLine(0.0f, 18.0f);


      // ========================================================
      // Deformed Shape
      // ========================================================

      if (ImGui::BeginMenu("Deformed Shape"))
        {
	  if (ImGui::MenuItem("Deformation X")) { world->render_flags ^= SHOW_DEFORM_X; }
	  if (ImGui::MenuItem("Deformation Y")) { world->render_flags ^= SHOW_DEFORM_Y;}
	  if (ImGui::MenuItem("Deformation Z")) { world->render_flags ^= SHOW_DEFORM_Z;}
	  ImGui::Separator();
	  if (ImGui::MenuItem("Rotation X")) { world->render_flags ^= SHOW_ROT_X;}
	  if (ImGui::MenuItem("Rotation Y")) { world->render_flags ^= SHOW_ROT_Y;}
	  if (ImGui::MenuItem("Rotation Z")) { world->render_flags ^= SHOW_ROT_Z;}
	  ImGui::Separator();
	  if (ImGui::MenuItem("Total Displacement")) {
	    world->render_flags |= (SHOW_DEFORM_X | SHOW_DEFORM_Y | SHOW_DEFORM_Z | SHOW_ROT_X | SHOW_ROT_Y | SHOW_ROT_Z);
	  }

	  ImGui::EndMenu();
	  
        }


      // ========================================================
      // Stresses
      // ========================================================

      if (ImGui::BeginMenu("Stresses"))
        {
	  if (ImGui::MenuItem("Axial Stress")) { world->render_flags ^= SHOW_AXIAL_STRESS;}
	  if (ImGui::MenuItem("Bending Stress X")) {world->render_flags ^= SHOW_BEND_STRESS_Y;}
	  if (ImGui::MenuItem("Bending Stress Y")) {world->render_flags ^= SHOW_BEND_STRESS_Z;}
	  if (ImGui::MenuItem("Normal Stress (Combined)")) {
	    world->render_flags |= (SHOW_AXIAL_STRESS |
				    SHOW_BEND_STRESS_Y |
				    SHOW_BEND_STRESS_Z);
	  }
	  if (ImGui::MenuItem("Shear Stress")) {world->render_flags ^= SHOW_SHEAR_STRESS;}
	  ImGui::Separator();
	  if (ImGui::MenuItem("Combined / Von Mises")) {world->render_flags ^= SHOW_VON_MISES;}
	  ImGui::EndMenu();
        }


      // ========================================================
      // Diagrams
      // ========================================================

      if (ImGui::BeginMenu("Diagrams"))
        {
	  if (ImGui::MenuItem("Axial Force (N)")) {
	    updateDiagram(world,renderer,model,DIAGRAM_N);
	  }

	  ImGui::Separator();

	  if (ImGui::MenuItem("Shear Vy")) {updateDiagram(world,renderer,model,DIAGRAM_VY);}
	  if (ImGui::MenuItem("Shear Vz")) {updateDiagram(world,renderer,model,DIAGRAM_VZ);}

	  ImGui::Separator();
	  
	  if (ImGui::MenuItem("Moment My")) {
	    updateDiagram(world,renderer,model,DIAGRAM_MY);
	  }
	
	  if (ImGui::MenuItem("Moment Mz")) {
	    updateDiagram(world,renderer,model,DIAGRAM_MZ);
	  }
	  if (ImGui::MenuItem("Torque Mx")) {
	    updateDiagram(world,renderer,model,DIAGRAM_T);
	  }

	  ImGui::EndMenu();
        }

      if(model->fea_solved){
	ImGui::SameLine(0.0f, 18.0f);
	ImGui::Separator();
	ImGui::SameLine(0.0f, 18.0f);

	bool inspectWasActive = world->inspect_active;

	if (inspectWasActive)
	  ImGui::PushStyleColor(
				ImGuiCol_Button,
				ImVec4(0.06f, 0.62f, 0.20f, 1.0f)
				);

	if (ImGui::Button("Inspect"))
	  world->inspect_active = !world->inspect_active;

	if (inspectWasActive)
	  ImGui::PopStyleColor();
      }
      ImGui::EndMainMenuBar();
    }

  inspectionFunction(world,model);
    
  if(world->render_flags != old_flags){
    for(int i=0;i<model->num_beam_elements;i++){
      createElementVertices(model,&(model->beams[i]),&(world->meshes[i]),world->render_flags);
    }
  }
}

float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
 
void inspectionFunction(World* world,FeaModel* model){
  if(world->inspect_active) {
    // do your raycast every frame here
    // store result in world->inspect_result
    //TODO: THE RAY TRACING LOGIC SHOULD BE HERE
    //NOTE: for now,just inspect the z displacement values
    float min_dis = 1e9;
    int element = -1;
    float xi = 0.0;
    for(int i=0;i<model->num_beam_elements;i++){
      float distance = 0.0;
      vec3 node1 = {model->nodes[model->beams[i].node1].x,model->nodes[model->beams[i].node1].y,model->nodes[model->beams[i].node1].z};
      vec3 node2 = {model->nodes[model->beams[i].node2].x,model->nodes[model->beams[i].node2].y,model->nodes[model->beams[i].node2].z};
      float x = rayInspection(world,ImGui::GetIO().MousePos.x,ImGui::GetIO().MousePos.y,node1,node2,&distance);
      if(distance < min_dis){
	min_dis = distance;
	element = i;
	xi = x;
      }
    }
    if(element == -1){
      return;
    }
    xi = clampf(xi,0.0,1.0);
    float threshold = sqrtf(model->sections[model->beams[element].sec_id].A) * 0.75f;
    if(min_dis <= threshold && (element != -1)) {
      BeamElement* beam = &(model->beams[element]);
      float dz = get_z_displacement(beam,xi,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
      //ImGui::SetNextWindowPos(
      //			      ImVec2(ImGui::GetIO().MousePos.x + 15,
      //				     ImGui::GetIO().MousePos.y + 15)
      //			      );
      ImGui::SetNextWindowPos(
			      ImVec2(world->width-250,
				     100)
			      );
      ImGui::SetNextWindowSize(ImVec2(220, 0)); // 0 = auto height
      ImGui::SetNextWindowBgAlpha(0.85f);
      ImGui::Begin("##inspect", NULL,
		   ImGuiWindowFlags_NoTitleBar |
		   ImGuiWindowFlags_NoResize |
		   ImGuiWindowFlags_NoMove |
		   ImGuiWindowFlags_NoSavedSettings |
		   ImGuiWindowFlags_NoFocusOnAppearing |
		   ImGuiWindowFlags_NoNav
		   );
        
      ImGui::TextColored(ImVec4(0.4f,0.8f,1.0f,1.0f), "Element %d  xi=%.3f",
			 element,xi);
      ImGui::Separator();
      ImGui::Text("DZ   = %.8f m",dz);
      ImGui::End();
    }else{
      ImGui::SetNextWindowPos(
			      ImVec2(world->width-250,
				     100)
			      );
      ImGui::SetNextWindowSize(ImVec2(220, 0)); // 0 = auto height
      ImGui::SetNextWindowBgAlpha(0.85f);
      ImGui::Begin("##inspect", NULL,
		   ImGuiWindowFlags_NoTitleBar |
		   ImGuiWindowFlags_NoResize |
		   ImGuiWindowFlags_NoMove |
		   ImGuiWindowFlags_NoSavedSettings |
		   ImGuiWindowFlags_NoFocusOnAppearing |
		   ImGuiWindowFlags_NoNav
		   );
        
      ImGui::TextColored(ImVec4(0.4f,0.8f,1.0f,1.0f), "Out of Range");
      ImGui::End();
    }
  }
}

void imgui_render(VkCommandBuffer command_buffer)
{
    ImGui::Render();   // <- was just Render()
    ImGui_ImplVulkan_RenderDrawData(
        ImGui::GetDrawData(),   // <- was just GetDrawData()
        command_buffer
    );
}


void imgui_draw_legend(float minVal, float maxVal)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = 24.0f, height = 200.0f;

    // Draw a vertical gradient rectangle (blue -> red, matching your stress color mapping)
    for (int i = 0; i < (int)height; i++)
    {
        float t = i / height;   // 0 at top, 1 at bottom
        ImU32 color = IM_COL32((int)(t * 255), 0, (int)((1.0f - t) * 255), 255);
        drawList->AddLine(
            ImVec2(pos.x, pos.y + i),
            ImVec2(pos.x + width, pos.y + i),
            color
        );
    }

    ImGui::Dummy(ImVec2(width, height)); // reserve layout space for the drawn rect

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Text("%.2f", maxVal);
    ImGui::Dummy(ImVec2(0, height - 40));
    ImGui::Text("%.2f", minVal);
    ImGui::EndGroup();
}

void imgui_draw_side_panel(bool hasSolved, float viewportHeight)
{
    if (!hasSolved) return;   // only shows after solving

    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight())); // just below the top bar
    ImGui::SetNextWindowSize(ImVec2(260, viewportHeight - ImGui::GetFrameHeight()));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
                            | ImGuiWindowFlags_NoCollapse
                            | ImGuiWindowFlags_NoResize;

    ImGui::Begin("Results", nullptr, flags);

    ImGui::Text("Node/Element under cursor:");
    ImGui::Separator();
    // hover data goes here — see part 2

    ImGui::End();
}

