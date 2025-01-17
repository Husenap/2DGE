#pragma once

#include "Buffer.h"
#include "Core/Constants.h"
#include "Core/Math/Mat4.h"
#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"
#include "Window/Window.h"

namespace Wraith
{
    class SpriteRenderer
    {
    public:
        SpriteRenderer(Window& window);
        ~SpriteRenderer();

        bool Init();

        void Submit(const SpriteCommand& sprite);
        void Submit(const SetCameraCommand& command);
        void Render();

        void Flip();

    private:
        Window& m_Window;

        struct RenderCamera
        {
            Mat4f projection;
            Mat4f view;
        };

        std::unique_ptr<RenderCamera> m_CurrentCamera;

        Shader m_SpriteShader;

        Buffer m_IndexBuffer;
        Buffer m_VertexBuffer;
        Buffer m_InstanceBuffer;

        struct ConstantBufferData
        {
            Mat4f view_projection;
            Mat4f projection;
            Vec2f resolution;
        } m_ConstantBufferData;
        Buffer m_ConstantBuffer;

        struct InstanceData
        {
            Vec4f color;
            Vec2f position;
            Vec2f offset;
            Vec2f scale;
            Vec2f size;
            f32 rotation;
            i32 world_space;
        };

        void UpdateConstantBuffer();

        // I found 1024 instances to be the best in terms of performance and drawcalls
        constexpr static u32 InstanceCount = 1024;
        std::vector<SpriteCommand> m_SpriteCommands[FRAME_COUNT];

        Sampler m_Sampler;
    };
}  // namespace Wraith