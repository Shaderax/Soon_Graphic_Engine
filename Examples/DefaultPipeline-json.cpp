// Load CubeMesh
// Load CubeTextureMine
// CubeMesh->GetMaterial().SetPipeline("Default.json");
// CubeMesh->GetMaterial()->SetTexture(CubeTextureMine);

// Load CubeMesh
// Load CubeTexture
// CubeMesh->GetMaterial().SetPipeline("Skybox.json");
// CubeMesh->GetMaterial()->SetTexture(CubeTexture);

// Pour le Compute
// pip = GraphicRenderer::AddPipeline("Compute.json");
// pip.AddToPipeline(MeshId); // Non un compute a pas ca.
// Et un particle sys a ca ? Un particle sys c'est une class avec un compute et un graphic.
// Les ComputesPipeline sont dans le renderer ?
   // Dans le cas ou le pc veut juste faire une texture bah clairement pas
// Les 2 sont possible avec un dispatch et un autre avec le bind etc

// Genre pour un nbody
// Puis je avoir un compute sans affichage ?
// Me faut une classe particleSystem qui détient une Compute pipeline

// 2 materiaux diffrrent pour 2 pipeline differente.
// ComputeShader.SetAmount(); -> psys.SetAmount() -> pip.Set(name, data) -> 
// Ou est save le nombre de particle qu'on doit afficher avec la compute ?
// C'est pas propre au particle sys mais au lieu du idRender je fais une struct pour les work item

// In the Json Set the default struct as particle or as coputed to rise up the number of element.
// Et pour la partie de rendu, deja on fait quoi si ya 2 de rendu dans la pipeline ?
// La graphic pipeline peut recevoir la description du buffer de la compute comme un VertexDescription et en fonction de ca faire les inputs pour la pipeline.

/*
----
layout(set = 0, binding = 1) buffer Bu
{
	Particle  particles[];
};
----

struct Buffer
{
	uint32_t bufferId;
};

pipCompute = AddPipeline("Compute.json");
pipNbody = AddPipeline("Nbody.json");

pipCompute.Dispatch();
pipCompute.GetBuffer("particles"); // BUFFER()

LIST(POSITION: "particles.pos")
Mesh mesh = buffer.ToMesh(LIST);
	// mesh.AddVertexElement(Position, vec3);

Dans le cas d'un particule sys on veut que les pos soit recup dans un uniform puis la pos est ajouter a l'objet 3d.

Mais la pos dans l'uniform arrive 

Si ya des sphere comment que je fais ?
inPos = Sphere.pos
inTexCoord = Sphere.texCoord
Et pour les pos ?
bah en théorie c'est un vertex en plus, mais on oeut pas et tout ne peut etre dans un meme buffer 

// Genre il recup les offset et les stride depuis l'uniform.

// Je recup une mesh avec le vertexElement de rempli et la mesh et le buffer ont le meme id ? OUI
// Et pour l'index Buffer  ?

Mesh BufferToMesh(BUFFER)
{
	mesh;
}

//TODO: If user setCopute on graphicPipeline

*/

/**
 * POST-PROCESSING
 */

 // Le post process s'applique sur toute l'image, mais dans le cas du bloom ya des liaison de framebufferdonc pas toute l'image.

 // Le mieux serait de pouvoir enlever le post process au runtime, genre chaque pipeline a un post process en vector et on peut ajouter dedans ou pas.

 // GraphicRenderer::AddPostProcessing();

 // Faut juste que je donne le pouvoir a l'user
 // pip = GraphicRenderer::AddPipeline("Hdr.json");
 // conf = pip->GetConf();
 // 

 /*
	Les cas :
		- On veut tout simplement afficher un object
		- On veut afficher un object et une skybox
		- On veut faire du hdr sur l'image
			- render pass classique en offscreen
			- 2e render pass pour l'hdr qui recoit en param le framebuffer
			- 3e render pass du resultat de l'hdr pour rendu final.

		donc soit je dois gerer a la main les frma buffer offscreen
		soit je laisse l'utilisateur le faire

		En gros on va pas les gere pour l'instant mais faire comme, du coup comment ca se passe dans le JSON ?
			- 
 */