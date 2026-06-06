import { KNOWLEDGEBASE } from '../../helpers/constants';
import { deleteKnowledgeBaseCategoryDtlWrapper, getknowledgeBaseArticleDtlHandlerWrapper, getknowledgeBaseDtlHandlerWrapper, insertKnowledgeBaseHandlerWrapper, updateKnowledgeBaseCategoryDtlWrapper, updateMoveFilesToFolderWrapper } from './handler';

export default function knowledgeBaseHandler(server: any, options: any, next: any) {
  server.post(
    KNOWLEDGEBASE.INSERT_KNOWLEDGE_BASE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create knowledge base',
        description: 'create knowledge base Api',
        tags: ['Knowledge base'],
      },
    },
    insertKnowledgeBaseHandlerWrapper
  );

  server.get(
    KNOWLEDGEBASE.GET_KNOWLEDGEBASE_CATEGORY_ARTICLE_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get knowledge base category article detials',
        description: 'Get knowledge base category article detials Api',
        tags: ['Knowledge base'],
      },
    },
    getknowledgeBaseDtlHandlerWrapper
  );

  server.get(
    KNOWLEDGEBASE.GET_KNOWLEDGEBASE_ARTICLE_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get knowledge base article details',
        description: 'Get knowledge base article details Api',
        tags: ['Knowledge base'],
      },
    },
    getknowledgeBaseArticleDtlHandlerWrapper
  );

  server.post(
    KNOWLEDGEBASE.UPDATE_KNOWLEDEBASE_CATEGORY_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update knowledge base category details',
        description: 'Update knowledge base category details Api',
        tags: ['Knowledge base'],
      },
    },
    updateKnowledgeBaseCategoryDtlWrapper
  );

  server.post(
    KNOWLEDGEBASE.DELETE_KNOWLEDGEBASE_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete knowledge base details',
        description: 'Delete knowledge base details Api',
        tags: ['Knowledge base'],
      },
    },
    deleteKnowledgeBaseCategoryDtlWrapper
  );

  server.post(
    KNOWLEDGEBASE.UPDATE_MOVE_FILES_TO_FOLDER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update move files to folder',
        description: 'Update move files to folder Api',
        tags: ['Knowledge base'],
      },
    },
    updateMoveFilesToFolderWrapper
  );

  next();
}
