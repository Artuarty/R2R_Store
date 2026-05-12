# Guía para Contribuir al Proyecto
Si deseas contribuir a este proyecto, por favor sigue estos pasos:

1.  **Haz un Fork:** Crea un fork de este repositorio en tu cuenta de Codigo Fuente.

2.  **Crea una Rama:** Clona tu fork localmente y crea una nueva rama para tus cambios, siguiendo el flujo de trabajo de **[Git Flow](https://docs.github.com/en/get-started/using-github/github-flow)**:    
    * Nueva funcionalidad: 
      ```bash
      git checkout -b feature/nombre-de-la-funcionalidad
      ```
    * Corrección:
      ```bash 
      git checkout -b fix/descripcion-del-problema
      ```

3.  **Realiza tus Cambios:** Implementa tu funcionalidad o corrección. Asegúrate de seguir los estándares de desarrollo del proyecto.

4.  **Haz Commit:** Guarda tus cambios con un mensaje claro y descriptivo, utilizando la convención **[Conventional Commits](https://www.conventionalcommits.org/)**.
    * Nueva funcionalidad: 
      ```bash
      git add .
      git commit -m "feat: descripción de la nueva funcionalidad"
      ```
    * Corrección:
       ```bash
       git add .
       git commit -m "fix: descripción de la corrección"
       ```

5.  **Empuja tus Cambios (Push):** Sube tu rama a tu fork en Codigo Fuente:
    ```bash
    git push origin feature/nombre-de-la-funcionalidad
    ```
6.  **Abre un Merge Request:** Desde tu fork en Codigo Fuente, crea un Merge Request hacia la rama principal **develop** del repositorio original. Describe claramente tus cambios, incluyendo el propósito, contexto y cualquier detalle técnico relevante.