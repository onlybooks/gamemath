
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 격자를 그리는 함수
void SoftRenderer::DrawGizmo2D()
{
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    // 그리드 색상
    LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

    // 뷰의 영역 계산
    Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
    Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

    // 좌측 하단에서부터 격자 그리기
    int xGridCount = _ScreenSize.X / _Grid2DUnit;
    int yGridCount = _ScreenSize.Y / _Grid2DUnit;

    // 그리드가 시작되는 좌하단 좌표 값 계산
    Vector2 minPos = viewPos - extent;
    Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
    ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

    for (int ix = 0; ix < xGridCount; ++ix)
    {
        r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
    }

    for (int iy = 0; iy < yGridCount; ++iy)
    {
        r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
    }

    ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
    r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
    r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 목록
static const std::string PlayerGo("Player");

// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene2D()
{
    // 최초 씬 로딩에서 사용하는 모듈 내 주요 레퍼런스
    auto& g = Get2DGameEngine();

    // 플레이어의 생성과 설정
    constexpr float playerScale = 30.f;
    GameObject& goPlayer = g.CreateNewGameObject(PlayerGo);
    goPlayer.SetMesh(GameEngine::QuadMesh);
    goPlayer.GetTransform().SetScale(Vector2::One * playerScale);
    goPlayer.SetColor(LinearColor::Red);

    // 100개의 배경 게임 오브젝트 생성과 설정
    char name[64];
    constexpr float squareScale = 20.f;
    std::mt19937 generator(0);
    std::uniform_real_distribution<float> dist(-1000.f, 1000.f);
    for (int i = 0; i < 100; ++i)
    {
        std::snprintf(name, sizeof(name), "GameObject%d", i);
        GameObject& newGo = g.CreateNewGameObject(name);
        newGo.GetTransform().SetPosition(Vector2(dist(generator), dist(generator)));
        newGo.GetTransform().SetScale(Vector2::One * squareScale);
        newGo.SetMesh(GameEngine::QuadMesh);
        newGo.SetColor(LinearColor::Blue);
    }
}

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
    // 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
    auto& g = Get2DGameEngine();
    const InputManager& input = g.GetInputManager();

    // 게임 로직의 로컬 변수
    static float moveSpeed = 200.f;
    static float rotateSpeed = 180.f;
    static float scaleMin = 15.f;
    static float scaleMax = 30.f;
    static float scaleSpeed = 180.f;
    static float minDistance = 1.f;
    static float lerpSpeed = 2.f;

    // 플레이어에 대한 주요 레퍼런스
    GameObject& goPlayer = g.GetGameObject(PlayerGo);
    TransformComponent& transform = goPlayer.GetTransform();

    // 입력에 따른 플레이어 위치와 크기의 변경
    transform.AddPosition(Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize() * moveSpeed * InDeltaSeconds);
    float newScale = Math::Clamp(transform.GetScale().X + scaleSpeed * input.GetAxis(InputAxis::ZAxis) * InDeltaSeconds, scaleMin, scaleMax);
    transform.SetScale(Vector2::One * newScale);
    transform.AddRotation(input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds);

    // 플레이어를 따라다니는 카메라의 트랜스폼
    TransformComponent& cameraTransform = g.GetMainCamera().GetTransform();
    Vector2 playerPos = transform.GetPosition();
    Vector2 cameraPos = cameraTransform.GetPosition();
    if ((playerPos - cameraPos).SizeSquared() < minDistance * minDistance)
    {
        cameraTransform.SetPosition(playerPos);
    }
    else
    {
        float ratio = Math::Clamp(lerpSpeed * InDeltaSeconds, 0.f, 1.f);
        Vector2 newCameraPos = cameraPos + (playerPos - cameraPos) * ratio;
        cameraTransform.SetPosition(newCameraPos);
    }
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
    // 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();
    const auto& texture = g.GetTexture(GameEngine::BaseTexture);

    // 배경에 격자 그리기
    DrawGizmo2D();

    // 렌더링 로직의 로컬 변수
    size_t totalObjectCount = g.GetScene().size();
    Matrix3x3 viewMatrix = g.GetMainCamera().GetViewMatrix();

    // 씬을 구성하는 모든 게임 오브젝트의 순회
    for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
    {
        // 게임 오브젝트의 레퍼런스를 얻기
        const GameObject& gameObject = *(*it);
        if (!gameObject.HasMesh() || !gameObject.IsVisible())
        {
            continue;
        }

        // 렌더링에 필요한 게임 오브젝트의 주요 레퍼런스를 얻기
        const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
        const TransformComponent& transform = gameObject.GetTransform();
        Matrix3x3 finalMatrix = viewMatrix * transform.GetModelingMatrix();

        // 게임 오브젝트의 렌더링 수행
        DrawMesh2D(mesh, finalMatrix, gameObject.GetColor());

        // 플레이어의 정보를 화면에 출력
        if (gameObject == PlayerGo)
        {
            r.PushStatisticText("Player Position : " + transform.GetPosition().ToString());
            r.PushStatisticText("Player Rotation : " + std::to_string(transform.GetRotation()) + " (deg)");
            r.PushStatisticText("Player Scale : " + std::to_string(transform.GetScale().X));
        }
    }
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
    // 메시의 구조를 파악하기 위한 로컬 변수
    size_t vertexCount = InMesh.GetVertices().size();
    size_t indexCount = InMesh.GetIndices().size();
    size_t triangleCount = indexCount / 3;

    // 메시 정보를 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
    std::vector<Vertex2D> vertices(vertexCount);
    std::vector<size_t> indice(InMesh.GetIndices());
    for (size_t vi = 0; vi < vertexCount; ++vi)
    {
        vertices[vi].Position = InMesh.GetVertices()[vi];
        if (InMesh.HasColor())
        {
            vertices[vi].Color = InMesh.GetColors()[vi];
        }

        if (InMesh.HasUV())
        {
            vertices[vi].UV = InMesh.GetUVs()[vi];
        }
    }

    // 정점 변환 진행
    VertexShader2D(vertices, InMatrix);

    // 그리기모드 설정
    FillMode fm = FillMode::None;
    if (InMesh.HasColor())
    {
        fm |= FillMode::Color;
    }
    if (InMesh.HasUV())
    {
        fm |= FillMode::Texture;
    }

    // 메시를 삼각형으로 쪼개서 각각 그리기
    for (int ti = 0; ti < triangleCount; ++ti)
    {
        int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
        std::vector<Vertex2D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };
        DrawTriangle2D(tvs, InColor, fm);
    }
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
    // 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
    auto& r = GetRenderer();
    const GameEngine& g = Get2DGameEngine();
    const Texture& texture = g.GetTexture(GameEngine::BaseTexture);

    if (IsWireframeDrawing())
    {
        // 와이어프레임으로 메시를 그리기
        LinearColor finalColor = _WireframeColor;
        if (InColor != LinearColor::Error)
        {
            finalColor = InColor;
        }

        r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
        r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
        r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
    }
    else
    {
        // 삼각형 칠하기
        // 삼각형의 영역 설정
        Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
        Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

        // 무게중심좌표를 위해 점을 벡터로 변환
        Vector2 u = InVertices[1].Position - InVertices[0].Position;
        Vector2 v = InVertices[2].Position - InVertices[0].Position;

        // 공통 분모 값 ( uu * vv - uv * uv )
        float udotv = u.Dot(v);
        float vdotv = v.Dot(v);
        float udotu = u.Dot(u);
        float denominator = udotv * udotv - vdotv * udotu;

        // 퇴화 삼각형이면 그리기 생략
        if (denominator == 0.f)
        {
            return;
        }

        float invDenominator = 1.f / denominator;

        ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
        ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

        // 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
        lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
        lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
        upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
        upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

        // 삼각형 영역 내 모든 점을 점검하고 색칠
        for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
        {
            for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
            {
                ScreenPoint fragment = ScreenPoint(x, y);
                Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
                Vector2 w = pointToTest - InVertices[0].Position;
                float wdotu = w.Dot(u);
                float wdotv = w.Dot(v);

                float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
                float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
                float oneMinusST = 1.f - s - t;
                if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
                {
                    Vector2 targetUV = InVertices[0].UV * oneMinusST + InVertices[1].UV * s + InVertices[2].UV * t;
                    r.DrawPoint(fragment, FragmentShader2D(texture.GetSample(targetUV), LinearColor::White));
                }
            }
        }
    }
}
