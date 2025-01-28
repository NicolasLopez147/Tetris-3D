# 🎮 Tetris 3D en C++ 

## Groupe
- Nicolas LOPEZ
- Nicolas RINCON

📖 **ENSTA** – Projet de programmation orientée objet IN204

## 📌 Description du projet  
Ce projet est une implémentation d’un **Tetris en trois dimensions** développé en **C++** avec **OpenGL** pour le rendu graphique. Il repose sur une architecture orientée objet, permettant une séparation claire des responsabilités entre les différentes classes.

L'objectif est de créer une expérience fluide et immersive en 3D tout en conservant les mécaniques classiques du Tetris.

## 🏗️ Architecture du projet  
Le jeu est organisé en plusieurs classes principales :

- **`Game`** : Gère la logique du jeu, le score et la progression.  
- **`Grid`** : Représente la grille de jeu et détecte les collisions.  
- **`Tetromino`** : Définit les différentes pièces et leurs rotations en 3D.  
- **`Shader` / `TextShader`** : Gèrent les shaders OpenGL et l’affichage du texte.  
- **`Menu` / `HowToPlayScreen`** : Assurent l’interface utilisateur et les interactions.  
- **`Renderer`** : S’occupe du rendu graphique des éléments du jeu.

Le jeu suit une boucle principale où chaque frame met à jour l’état du jeu et effectue le rendu en OpenGL.

## 🖥️ Technologies utilisées  
- **Langage** : C++  
- **Bibliothèques** : OpenGL, GLFW, GLM  
- **Programmation orientée objet (POO)**  
- **Shaders GLSL** pour les effets graphiques et l'affichage du texte

## 🎯 Fonctionnalités principales  
✅ Déplacement et rotation des tétraminos en 3D  
✅ Détection des collisions et gestion de la grille  
✅ Interface utilisateur avec un menu et un écran d’aide  
✅ Affichage du score en temps réel  
✅ Gestion des shaders pour un rendu optimisé  
---

💡 **Exécution !** g++ main.cpp -o main -lGL -lGLU -lglut -lfreetype -lGLEW -lglfw -I/usr/include/freetype2
