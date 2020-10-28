/*
@file EnemyActor.cpp
*/
#include "EnemyActor.h"
#include <glm/gtc/matrix_transform.hpp>
#include<algorithm>

/*
�R���X�g���N�^

@param hm     �G�l�~�[�̒��n����Ɏg�p���鍂���}�b�v
@param buffer �G�l�~�[�̃��b�V���f�[�^�������b�V���o�b�t�@
@param pos    �G�l�~�[�̏������W
@param rot    �G�l�~�[�̏�������
*/
EnemyActor::EnemyActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	: SkeletalMeshActor(buffer.GetSkeletalMesh("oni_small"), "Enemy", 3, pos, rot),
	heightMap(hm)
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);
	GetMesh()->Play("Idle");
	state = State::idle;
}
/*
�X�V
@param deltaTime �o�ߎ���
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
				actionTimer = 1.5;
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


	//���W�̍X�V
	SkeletalMeshActor::Update(deltaTime);
	if (attackCollision) {
		attackCollision->Update(deltaTime);
	}

	//�ڒn����
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y <= groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
		isInAir = false;
	}
	else if (position.y > groundHeight) {
		//����Ă��镨�̂��痣�ꂽ��󒆔���ɂ���
		if (boardingActor) {
			if (boardingActor->health <= 0) {
				boardingActor.reset();
			}
			else {
				Collision::Shape col = colWorld;
				col.s.r += 0.1f;//�Փ˔���������傫������
				glm::vec3 pa, pb;
				if (!Collision::TestShapeShape(col, boardingActor->colWorld, &pa, &pb)) {
					boardingActor.reset();
				}
			}
		}
		//��������
		const bool isFloating = position.y > groundHeight + 0.1f; //�n�ʂ��畂���Ă��邩
		if (!isInAir && isFloating && !boardingActor) {
			isInAir = true;
		}
		//�d�͂�������
		if (isInAir) {
			velocity.y -= gravity * deltaTime;
		}
	}
	if (!dead) {
		//�A�j���[�V�����̍X�V
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
			const float horizontalSpeed = velocity.x* velocity.x + velocity.z * velocity.z;

			
			if (horizontalSpeed == 0 || nowAction == 0) {
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
�Փ˃n���h��
@param b �Փ˔���̃A�N�^�[
@param p �Փ˂������������W
*/
void EnemyActor::OnHit(const ActorPtr& b, const glm::vec3& p)
{
	const glm::vec3 v = colWorld.s.center - p;
	//�Փˈʒu�Ƃ̋������߂����Ȃ������ׂ�
	if (dot(v, v) > FLT_EPSILON) {
		//this��b�ɏd�Ȃ�Ȃ��ʒu�܂ňړ�
		const glm::vec3 vn = normalize(v);
		float radiusSum = colWorld.s.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;

		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		position += vn * distance;
		colWorld.s.center += vn * distance;
	}
	else {
		//�ړ���������(�������߂�����ꍇ�̗�O����)
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
	glm::vec3 v = TAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//�^�[�Q�b�g�܂ł̋���

	return dist;
}


/*
�U���������������
*/
void EnemyActor::Attack()
{
	if (!attackCollision) {
			nowAttack = true;
			/*GetMesh()->Play("Attack.Kick", false);*/
			GetMesh()->Play("MoveToLeft", false);
			attackTimer = 0.0f;
			velocity = glm::vec3(0);
			state = State::attack;
	}
}


/*
�G�l�~�[������Ă��镨�̂�ݒ肷��

@param p ����Ă��镨��
*/
void EnemyActor::SetBoardingActor(ActorPtr p)
{
	boardingActor = p;
	if (p) {
		isInAir = false;
	}
}

float getRadian(float x, float z, float x2, float z2) {
	float rad = std::atan2(z2 - z, x2 - x);
	return rad;
}

void EnemyActor::Inactive()
{
	GetMesh()->Play("Idle");
	state = State::inactive;

	glm::vec3 v = TAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//�^�[�Q�b�g�܂ł̋���
	glm::vec3 direction = glm::normalize(v);//�^�[�Q�b�g�ւ̒P�ʃx�N�g��

	float target = std::atan2(-direction.z, direction.x) + glm::radians(90.0f);

	float A = rotation.y - 1.0f;
	float B = rotation.y + 1.0f;
	float C, D;
	float pi = M_PI;
	if (A <= -pi) {
		C = A + pi;
		D = pi - C;

	}

	if (dist <= 3) {
		if (A <= target || target <= B) {
			nowAction = 1;
		}
	}
	//BGM,��A�N�e�B�u����
	//player�̂�������ɓ����鐔�l���ړ������Ĕ͈͂̒lrotation.y����쐬���ČŒ�
}

/*
�ړ�����������
*/
void EnemyActor::Move()
{
	//�^�[�Q�b�g�ւ̃x�N�g�����v�Z
	glm::vec3 v = TAct->position - position;

	v.y = 0;

	float dist = glm::length(v);//�^�[�Q�b�g�܂ł̋���
 	move = glm::normalize(v);//�^�[�Q�b�g�ւ̒P�ʃx�N�g��

	if (!nowAttack) {
		if (dist <= 2) {
			Attack();
			nowAction = 3;
			return;
		}
	}

	//�ړ����s���Ă�����A�ړ������ɉ����Č����Ƒ��x���X�V
	if (glm::dot(move, move)) {
		//�������X�V
		move = glm::normalize(move);
		rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);

		//���̂ɏ���Ă��Ȃ��Ƃ��͒n�`�̌��z���l�����Ĉړ������𒲐�����
		if (!boardingActor) {
			//�ړ������̒n�`�̌��z(gradient)���v�Z
			const float minGradient = glm::radians(-60.0f); //�������Ƃ̂ł�����z�̍ŏ��l
			const float maxGradient = glm::radians(60.0f); //�������Ƃ̂ł�����z�̍ő�l
			const float frontY =
				heightMap->Height(position + move * 0.05f) - position.y - 0.01f;
			const float gradient =
				glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//�n�`�ɉ����悤�Ɉړ����x��ݒ�
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			move = glm::rotate(glm::mat4(1), gradient, axis) * glm::vec4(move, 1.0f);

		}
		velocity = move * moveSpeed;
	}
	else {
		//�ړ����Ă��Ȃ��̂ő��x��0�ɂ���
		velocity = glm::vec3(0);
	}

}
void EnemyActor::Feint() {
	//�G�l�~�[���v���C���[�̕���������悤�ɐݒ�
	glm::vec3 d = TAct->position - position;
	d.y = 0;
	glm::vec3 direction = glm::normalize(d);//�G�l�~�[�̑O���̒P�ʃx�N�g��

	if (!onlyOnce) {

		float pd=PlayerDist();
		int probability = rand() % 100;

		if (pd <= 3) {
			if (probability >= 60) {
				move = glm::normalize(-d);//�G�l�~�[�̌�����
				feintD = 0;
			}
			else if (probability >= 30) {
				move = glm::normalize(glm::vec3(-d.x, 0, d.z));//�G�l�~�[���猩�č��ړ�
				feintD = 1;
			}
			else {
				move = glm::normalize(glm::vec3(d.z, 0, -d.x));//�G�l�~�[���猩�ĉE�ړ�
				feintD = 2;
			}
		}
		else {
			if (probability >= 50) {
				move = glm::normalize(glm::vec3(-d.x, 0, d.z));//�G�l�~�[���猩�č��ړ�
				feintD = 1;
			}
			else{
				move = glm::normalize(glm::vec3(d.z, 0, -d.x));//�G�l�~�[���猩�ĉE�ړ�
				feintD = 2;
			}
		}
		
		//�ړ����ݒ�
		//move = glm::normalize(-d);//�G�l�~�[�̌���̒P�ʃx�N�g��
		targetPos = position + move * glm::vec3(2.0, 0, 2.0);
		
		onlyOnce = true;
	}
	glm::vec3 v = targetPos - position;
	v.y = 0;
	float dist = glm::length(v);//�ړ���̋���

	if (dist<= 0.5) {
		onlyOnce = false;
		nowAction = 1;
		return;
	}

	//�ړ����s���Ă�����A�ړ������ɉ����Č����Ƒ��x���X�V
	if (glm::dot(move, move)) {
		//�������X�V
		direction = glm::normalize(direction);
		rotation.y = std::atan2(-direction.z, direction.x) + glm::radians(90.0f);
		//���̂ɏ���Ă��Ȃ��Ƃ��͒n�`�̌��z���l�����Ĉړ������𒲐�����
		if (!boardingActor) {
			//�ړ������̒n�`�̌��z(gradient)���v�Z
			const float minGradient = glm::radians(-60.0f); //�������Ƃ̂ł�����z�̍ŏ��l
			const float maxGradient = glm::radians(60.0f); //�������Ƃ̂ł�����z�̍ő�l
			const float frontY =
				heightMap->Height(position + move * 0.05f) - position.y - 0.01f;
			const float gradient =
				glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//�n�`�ɉ����悤�Ɉړ����x��ݒ�
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			const glm::vec3 move2 = glm::rotate(glm::mat4(1), gradient, axis) * glm::vec4(move, 1.0f);
			velocity = move2 * feintSpped;
		}
		else {
			velocity = move * feintSpped;
		}
	}
	else {
		//�ړ����Ă��Ȃ��̂ő��x��0�ɂ���
		velocity = glm::vec3(0);
	}
	
}
void EnemyActor::Damage() {
	nowAction = 4;
	velocity = glm::vec3(0);
	nowAttack = false;
	state = State::damage;
}