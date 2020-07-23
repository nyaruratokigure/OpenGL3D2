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
	actionTimer -= deltaTime;
	if (actionTimer<0) {
		probability = rand() % 100;
		if (probability < 70) {
			actionTimer = 3;
			nowAction = 1;
			SRangeAttack();
			
			return;
		}
		else {
			actionTimer = 2;
			nowAction = 2;
			nowPosition = position;//���݂̃G�l�~�[�̈ʒu���R�s�[
			Feint();
			return;
		}
	}
	else {
		if (nowAction = 1) {
			SRangeAttack();
		}
		else {
			Feint();
		}
	}


	//���W�̍X�V.
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
		case State::run:
			if (isInAir) {
				GetMesh()->Play("Jump");
				state = State::jump;
			}
			else {
				const float horizontalSpeed = velocity.x*velocity.x + velocity.z*velocity.z;
				if (horizontalSpeed == 0) {
					GetMesh()->Play("Idle");
					state = State::idle;
				}
			}
			break;


		case State::idle:
			if (isInAir) {
				GetMesh()->Play("Jump");
				state = State::jump;
			}
			else {
				const float horizontalSpeed = velocity.x* velocity.x + velocity.z * velocity.z;
				if (horizontalSpeed != 0) {
					GetMesh()->Play("Run");
					state = State::run;
				}
			}
			break;

		case State::jump:
			if (!isInAir) {
				GetMesh()->Play("Idle");
				state = State::idle;
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
				nowAction = 0;
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


/*
�U���������������
*/
void EnemyActor::Attack()
{
	if (!attackCollision) {
			nowAttack = true;
			GetMesh()->Play("Attack", false);
			attackTimer = 0.0f;
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

/*
�ړ�����������
*/
void EnemyActor::SRangeAttack()
{
	if (nowAction == 1) {
		probability = rand() % 100;
		if (probability < 30) {
			nowAction = 2;
			nowPosition = position;//���݂̃G�l�~�[�̈ʒu���R�s�[
			Feint();
			return;
		}
	}
	else {
		Feint();
		return;
	}
	//�󒆂ɂ��鎞�͈ړ��ł��Ȃ�
	/*if (isInAir) {
		return;
	}*/
	if (dead) {
		velocity = glm::vec3(0);
	}
	else {


		//�^�[�Q�b�g�ւ̃x�N�g�����v�Z
		glm::vec3 v = TAct->position - position;

		v.y = 0;

		float dist = glm::length(v);//�^�[�Q�b�g�܂ł̋���
		glm::vec3 move = glm::normalize(v);//�^�[�Q�b�g�ւ̒P�ʃx�N�g��

		if (!nowAttack) {
			if (dist <= 2) {
				Attack();
				velocity = glm::vec3(0);
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
}

void EnemyActor::Feint() {
	
	if (!onlyOnce) {
		ver = rand() % 10 - 5;

		hor = rand() % 10 - 5;

		onlyOnce = true;
	    thisPos = nowPosition-glm::vec3(ver, 0, hor);
		 //thisPos.y = 0;
	}
	glm::vec3 v =thisPos - position;
	v.y = 0;
	float dist = glm::length(v);//�ړ���̋���
	glm::vec3 move = glm::normalize(v);//�ړ���ւ̒P�ʃx�N�g��

	if (dist<= 1) {
		nowAction = 0;
		onlyOnce = false;
		printf("�t�F�C���g�I");
		return;
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
		velocity = move * feintSpped;
	}
	else {
		//�ړ����Ă��Ȃ��̂ő��x��0�ɂ���
		velocity = glm::vec3(0);
	}
}

