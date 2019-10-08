# Direct State Access (DSA)

Examples of using direct state access methods which means that the object does not need be bound before the change but the state can be changed with the object name. This includes methods such as:
- glUniform -> glProgramUniform
- glClear -> glClearNamedFramebufferfv
- glTexStorage2D -> glTextureStorage2D
- glBufferStorage -> glNamedBufferStorage
- glVertexAttribPointer -> glVertexArrayAttribFormat