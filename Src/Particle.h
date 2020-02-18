/*
@file Particle.h
*/
#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include "BufferObject.h"
#include "Shader.h"
#include "Sprite.h"
#include <glm/glm.hpp>
#include <list>
#include <memory>

//��s�錾
class Particle;
class ParticleEmitter;
using ParticleEmitterPtr = std::shared_ptr<ParticleEmitter>;
class ParticleSystem;

/*
�p�[�e�B�N���̃p�����[�^�[
*/
struct ParticleParameter
{
	float lifetime = 1;							//��������
	glm::vec3 velocity = glm::vec3(0, 2, 0);	//���x
	glm::vec3 acceleration = glm::vec3(0);		//�����x
	glm::vec2 scale = glm::vec2(1);				//�傫��
	float rotation = 0;							//��]
	glm::vec4 color = glm::vec4(1);				//�F�ƕs�����x
};

/*
�p�[�e�B�N���E�G�~�b�^�[�̃p�����[�^�[
*/
struct ParticleEmitterParameter
{
	int id = 0;							//�G�~�b�^�[�𐧌䂷�邽�߂�ID
	glm::vec3 position = glm::vec3(0);	//�G�~�b�^�[�̈ʒu
	glm::vec3 rotation = glm::vec3(0);	//�G�~�b�^�[�̌���
	float duration = 1.0f;				//�p�[�e�B�N������o����b��
	bool loop = true;					//���[�v�Đ�����Ȃ�true�A��~����Ȃ�false
	float emissionsPerSecond = 5.0f;	//�b������̃p�[�e�B�N�����o��
	float angle = glm::radians(15.0f);	//���o�������`����~���̊p�x
	float radius = 0.5f;				//���o�������`����~���̔��a
	float gravity = 9.8f;				//�p�[�e�B�N���ɂ�����d��
	std::string imagePath;				//�e�N�X�`���E�t�@�C����
	
	glm::ivec2 tiles = glm::ivec2(1);	//�e�N�X�`���̏c�Ɖ��̕�����
	GLenum srcFactor = GL_SRC_ALPHA;
	GLenum dstFactor = GL_ONE_MINUS_SRC_ALPHA;
};

/*
�p�[�e�B�N��
*/
class Particle {
public:
	friend ParticleSystem;
	friend ParticleEmitter;

	Particle(const ParticleParameter& pp, const glm::vec3& pos, const Rect& r);
	virtual ~Particle() = default;

	void Update(float deltatime);
	bool IsDead() const { return lifetime <= 0; }

private:
	Rect rect = { glm::vec2(0), glm::vec2(0) }; //�e�N�X�`���\���͈�
	glm::vec3 position = glm::vec3(0);		//�ʒu
	float lifetime = 1;						//��������
	glm::vec3 velocity = glm::vec3(0, 2, 0);//���x
	glm::vec3 acceleration = glm::vec3(0);	//�����x
	glm::vec2 scale = glm::vec2(1);			//�傫��
	float rotation = 0;						//��]
	glm::vec4 color = glm::vec4(1);			//�F�ƕs�����x
};

/*
�p�[�e�B�N�����o�N���X
*/
class ParticleEmitter
{
public:
	friend ParticleSystem;

	ParticleEmitter(const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	~ParticleEmitter() = default;

	void Update(float deltatime);
	void Draw();

	//���W�̐ݒ�E�擾
	void Position(const glm::vec3& p) { ep.position = p; }
	const glm::vec3& Position()const { return ep.position; }

	bool IsDead() const { return !ep.loop&&timer >= ep.duration&&particles.empty(); }

private:
	void AddParticle();
	ParticleEmitterParameter ep; //�G�~�b�^�[�̃p�����[�^�[
	ParticleParameter pp;        //�p�[�e�B�N���̃p�����[�^�[
	
	Texture::Image2DPtr texture; //�p�[�e�B�N���p�e�N�X�`��
	float interval = 0;          //�p�[�e�B�N���̔����Ԋu(�b)
	float timer = 0;             //�o�ߎ���(�b)
	float emissionTimer = 0;     //�p�[�e�B�N�������^�C�}�[(�b)
	
	size_t count = 0;      //�`�悷��C���f�b�N�X��
	size_t baseVertex = 0; //�`��̊�ƂȂ钸�_�̃I�t�Z�b�g
	
	std::list<Particle>  particles; //�p�[�e�B�N�����X�g
};

/*
�p�[�e�B�N���Ǘ��N���X
*/
class ParticleSystem 
{
public:
	ParticleSystem() = default;
	~ParticleSystem() = default;

	bool Init(size_t maxParticlecount);
	ParticleEmitterPtr Add(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	ParticleEmitterPtr Find(int id) const;
	void Remove(const ParticleEmitterPtr&);
	void Clear();
	void Update(float deltatime);
	void Draw(const glm::mat4& matProj, const glm::mat4& matView);

private:
	BufferObject vbo;
	BufferObject ibo;
	VertexArrayObject vao;
	Shader::ProgramPtr program;
	std::list<ParticleEmitterPtr> emitters;
};


#endif //PARTICLE_H_INCLUDED
