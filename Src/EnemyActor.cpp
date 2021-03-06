/*
@file EnemyActor.cpp
*/
#include "EnemyActor.h"
#include "MainGameScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include<algorithm>

/*
コンストラクタ

@param hm     エネミーの着地判定に使用する高さマップ
@param buffer エネミーのメッシュデータを持つメッシュバッファ
@param ms     エネミーの反応を表すマークのポインタ
@param pos    エネミーの初期座標
@param rot    エネミーの初期方向
*/
EnemyActor::EnemyActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const MarkPtr& mp,const glm::vec3& pos, const glm::vec3& rot)
	: SkeletalMeshActor(buffer.GetSkeletalMesh("oni_small"), "Enemy", 3, pos, rot),
	heightMap(hm)
{
	colLocal = Collision::CreateCapsule(
		glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
	GetMesh()->Play("Idle.LookAround");
	state = State::inactive;
	markp = mp;
}
/*
更新
@param deltaTime 経過時間
+*/
void EnemyActor::Update(float deltaTime)
{
	if (dead) 
		return;

	if (nowAction == 0) {
		Inactive();
	}

	if (nowAction == 1) {
		actionTimer = 0;
	}
	if (actionTimer <= 0) {
		if (nowAction ==1||
			nowAction == 2||
			nowAction == 5){
			onlyOnce = false;
			//float dist = PlayerDist();
			int probability = rand() % 100;
			if (probability > 70) {
				actionTimer = 3;
				nowAction = 2;
			}
			else {
				actionTimer = 1.5f;
				nowAction = 5;
			}
		}
	}
	
	else {
		actionTimer -= deltaTime;
		if (nowAction == 2) {
			Move();
		}
		else if (nowAction == 5) {
			Feint();
		}
		/*else if (nowAction == 3) {
			Attack();
		}
		else {
			Damage();
		}*/
	}



	//座標の更新
	SkeletalMeshActor::Update(deltaTime);
	if (attackCollision) {
		attackCollision->Update(deltaTime);
	}

	//マークの更新

	if (active) {
		std::cout << "マーク表示した" << std::endl;
		markp->position.x = position.x;
		markp->position.z = position.z;
		markp->position.y = position.y + 2.0f;


		markp->Update(deltaTime);
	}

	//接地判定
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y <= groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
		isInAir = false;
	}
	else if (position.y > groundHeight) {
		//乗っている物体から離れたら空中判定にする
		if (boardingActor) {
			if (boardingActor->health <= 0) {
				boardingActor.reset();
			}
			else {
				Collision::Shape col = colWorld;
				col.s.r += 0.1f;//衝突判定を少し大きくする
				glm::vec3 pa, pb;
				if (!Collision::TestShapeShape(col, boardingActor->colWorld, &pa, &pb)) {
					boardingActor.reset();
				}
			}
		}
		//落下判定
		const bool isFloating = position.y > groundHeight + 0.1f; //地面から浮いているか
		if (!isInAir && isFloating && !boardingActor) {
			isInAir = true;
		}
		//重力を加える
		if (isInAir) {
			velocity.y -= gravity * deltaTime;
		}
	}
	if (!dead) {
		//アニメーションの更新
		switch (state) {
		case State::idle:
		{
			const float horizontalSpeed = velocity.x* velocity.x + velocity.z * velocity.z;
			if (horizontalSpeed != 0) {
				if (nowAction == 2) {
					GetMesh()->Play("Run");
					state = State::run;
				}
				else if(nowAction == 5) {
					if (feintD == 0) {
						GetMesh()->Play("Run");
						GetMesh()->SetAnimationSpeed(0.7);
					}
					if (feintD == 1) {
						GetMesh()->Play("MoveToLeft");
					}
					if (feintD == 2) {
						GetMesh()->Play("MoveToRight");
					}
					state = State::feint;
				}
			}
		}
		break;

		case State::inactive:
			if (nowAction != 0) { 
				active = true;
				GetMesh()->Play("Idle");
				state = State::idle;
				
			}

			break;


		case State::run:
		{
			const float horizontalSpeed = velocity.x*velocity.x + velocity.z*velocity.z;
			if (horizontalSpeed == 0) {
				GetMesh()->Play("Idle");
				state = State::idle;
			}
			
			if (nowAction == 5) {
				if (feintD == 0) {
					GetMesh()->Play("Run");
					GetMesh()->SetAnimationSpeed(0.7);
				}
				if (feintD == 1) {
					GetMesh()->Play("MoveToLeft");
				}
				if (feintD == 2) {
					GetMesh()->Play("MoveToRight");
				}
				state = State::feint;
			}

		}
		break;

		case State::attack:
			attackTimer += deltaTime;
			if (attackTimer > 0.45f && attackTimer < 1.00f) {
				if (!attackCollision) {
					static const float radian = 1.0f;
					const glm::vec3 front = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, 0.5f, 1);
					attackCollision = std::make_shared<Actor>("EnemyAttackCollision", 1, position + front + glm::vec3(0, 1, 0), glm::vec3(0), glm::vec3(radian));
					attackCollision->colLocal = Collision::CreateSphere(glm::vec3(0), radian);
				}
			}

			else {
				attackCollision.reset();
			}
			if (GetMesh()->IsFinished()) {
				nowAttack = false;
				attackCollision.reset();
				GetMesh()->Play("Idle");
				state = State::idle;
				nowAction = 1;
			}
			break;

		case State::damage:
			GetMesh()->SetAnimationSpeed(1);
			if (GetMesh()->IsFinished()) {
				state = State::idle;
				nowAction = 1;
			}
			break;

		case State::feint:

			if (nowAction == 1) {
				GetMesh()->Play("Idle");
				state = State::idle;
			}

			if (nowAction == 2) {
				GetMesh()->Play("Run");
				GetMesh()->SetAnimationSpeed(1);
				state = State::run;
			}
			break;
		}
	}
}

/*
衝突ハンドラ
@param b 衝突判定のアクター
@param p 衝突が発生した座標
*/
void EnemyActor::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	const glm::vec3 c = (colWorld.c.seg.a + colWorld.c.seg.b) / 2.0f;
	const glm::vec3 v = c - p;
	//衝突位置との距離が近すぎないか調べる
	if (dot(v, v) > FLT_EPSILON) {
		//thisをbに重ならない位置まで移動
		const glm::vec3 vn = normalize(v);
		float radiusSum = colWorld.c.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;

		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		position += vn * distance;
		colWorld.s.center += vn * distance;
	}
	else {
		//移動を取り消す(距離が近すぎる場合の例外処理)
		const float deltaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = velocity * deltaTime;
		position -= deltaVelocity;
		colWorld.s.center -= deltaVelocity;
	}
	if (v.y <= 0.5) {
		SetBoardingActor(b);
	}
}

float EnemyActor::PlayerDist() {
	glm::vec3 v = tAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//ターゲットまでの距離

	return dist;
}


/*
攻撃操作を処理する
*/
void EnemyActor::Attack()
{
	if (!attackCollision) {
			nowAttack = true;
			GetMesh()->Play("Attack.Kick", false);
			attackTimer = 0.0f;
			velocity = glm::vec3(0);
			state = State::attack;
	}
}


/*
エネミーが乗っている物体を設定する

@param p 乗っている物体
*/
void EnemyActor::SetBoardingActor(ActorPtr p)
{
	boardingActor = p;
	if (p) {
		isInAir = false;
	}
}

void EnemyActor::Inactive()
{
	glm::vec3 v = tAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//ターゲットまでの距離
	glm::vec3 target = glm::normalize(v);//ターゲットへの単位ベクトル

	float radian = std::atan2(-target.z, target.x)-glm::radians(90.0f);
	if (radian <= 0) {
		radian += glm::radians(360.0f);
	}
	if (std::abs(radian - rotation.y) > frontRange) {
		return;
	}
	else {
		nowAction = 1;
	}
}

	/*float A = rotation.y - 1.0f;
	float B = rotation.y + 1.0f;
	float C = 0;
	float pi = M_PI;
	targett = target + (pi / 2);
	if(pi<=targett){
		targett = targett - pi;
		targett = -pi + targett;
	}

	if (A <= -pi) {
		C = A + (pi*2);
	}
	if (pi <= B) {
		C = B - (pi * 2);
	}

	if (dist <= 3) {
		if (A <= targett&&targett<= B) {
			nowAction = 1;
		}
		else if (C < 0) {
			if (targett <= C) {
				nowAction = 1;
			}
		}
		else if (C > 0) {
			if (C <= targett) {
				nowAction = 1;
			}
		}
		
	}*/


/*
移動を処理する
*/
void EnemyActor::Move()
{
	//ターゲットへのベクトルを計算
	glm::vec3 v = tAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//ターゲットまでの距離
 	move = glm::normalize(v);//ターゲットへの単位ベクトル

	if (!nowAttack) {
		if (dist <= 2) {
			Attack();
			nowAction = 3;
			return;
		}
	}

	//移動が行われていたら、移動方向に応じて向きと速度を更新
	if (glm::dot(move, move)) {
		//向きを更新
		move = glm::normalize(move);
		rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);

		//物体に乗っていないときは地形の勾配を考慮して移動方向を調整する
		if (!boardingActor) {
			//移動方向の地形の勾配(gradient)を計算
			const float minGradient = glm::radians(-60.0f); //沿うことのできる勾配の最小値
			const float maxGradient = glm::radians(60.0f); //沿うことのできる勾配の最大値
			const float frontY =
				heightMap->Height(position + move * 0.05f) - position.y - 0.01f;
			const float gradient =
				glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//地形に沿うように移動速度を設定
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			move = glm::rotate(glm::mat4(1), gradient, axis) * glm::vec4(move, 1.0f);

		}
		velocity = move * moveSpeed;
	}
	else {
		//移動していないので速度を0にする
		velocity = glm::vec3(0);
	}

}
void EnemyActor::Feint() {
	//エネミーがプレイヤーの方向を見るように設定
	glm::vec3 d = tAct->position - position;
	d.y = 0;
	glm::vec3 direction = glm::normalize(d);//エネミーの前方の単位ベクトル

	if (!onlyOnce) {

		float pd = PlayerDist();
		int probability = rand() % 100;

		if (pd <= 3) {
			if (probability >= 60) {
				move = glm::normalize(-d);//エネミーの後ろ方向
				feintD = 0;
			}
			else if (probability >= 30) {
				move = glm::normalize(glm::vec3(-d.x, 0, d.z));//エネミーから見て左移動
				feintD = 1;
			}
			else {
				move = glm::normalize(glm::vec3(d.z, 0, -d.x));//エネミーから見て右移動
				feintD = 2;
			}
		}
		else {
			if (probability >= 50) {
				move = glm::normalize(glm::vec3(-d.x, 0, d.z));//エネミーから見て左移動
				feintD = 1;
			}
			else{
				move = glm::normalize(glm::vec3(d.z, 0, -d.x));//エネミーから見て右移動
				feintD = 2;
			}
		}
		
		//移動先を設定
		targetPos = position + move * glm::vec3(2.0f, 0, 2.0f);
		
		onlyOnce = true;
	}
	glm::vec3 v = targetPos - position;
	v.y = 0;
	float dist = glm::length(v);//移動先の距離

	if (dist<= 0.5) {
		onlyOnce = false;
		nowAction = 1;
		return;
	}

	//移動が行われていたら、移動方向に応じて向きと速度を更新
	if (glm::dot(move, move)) {
		//向きを更新
		direction = glm::normalize(direction);
		rotation.y = std::atan2(-direction.z, direction.x) + glm::radians(90.0f);
		//物体に乗っていないときは地形の勾配を考慮して移動方向を調整する
		if (!boardingActor) {
			//移動方向の地形の勾配(gradient)を計算
			const float minGradient = glm::radians(-60.0f); //沿うことのできる勾配の最小値
			const float maxGradient = glm::radians(60.0f); //沿うことのできる勾配の最大値
			const float frontY =
				heightMap->Height(position + move * 0.05f) - position.y - 0.01f;
			const float gradient =
				glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//地形に沿うように移動速度を設定
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			const glm::vec3 move2 = glm::rotate(glm::mat4(1), gradient, axis) * glm::vec4(move, 1.0f);
			velocity = move2 * feintSpped;
		}
		else {
			velocity = move * feintSpped;
		}
	}
	else {
		//移動していないので速度を0にする
		velocity = glm::vec3(0);
	}
	
}
void EnemyActor::Damage() {
	nowAction = 4;
	active = true;
	velocity = glm::vec3(0);
	nowAttack = false;
	state = State::damage;
}
void EnemyActor::Dead() {
	dead = true;
}