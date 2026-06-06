import { logger } from './plugins/log';
import { loggerError } from './plugins/logger';

export const conditionsMapper = (leftOperand: any, condition: any, rightOperand: any) => {
  try {
    let left: any;
    let right: any;
    const leftType = isNaN(Number(leftOperand));
    const rightType = isNaN(Number(rightOperand));
    if (!leftType && !rightType) {
      left = Number(leftOperand);
      right = Number(rightOperand);
    } else {
      left = leftOperand;
      right = rightOperand;
    }
    const mappedCondition = conditionalOperator[condition];
    switch (mappedCondition) {
      case 'Equal to': {
        return left === right;
      }
      case 'Greater than': {
        return left > right;
      }
      case 'Greater than or Equal to': {
        return left >= right;
      }
      case 'Less than': {
        return left < right;
      }
      case 'Less than or Equal to': {
        return left <= right;
      }
      case 'Starts with': {
        left = left.toString();
        right = right.toString();
        return left?.at(0) === right;
      }
      case 'Ends with': {
        left = left.toString();
        right = right.toString();
        return left?.at(-1) === right;
      }
      case 'Contains': {
        left = left.toString();
        right = right.toString();
        return left?.includes(right);
      }
      default: {
        return false;
      }
    }
  } catch (error) {
    loggerError('conditionsMapper', { leftOperand, condition, rightOperand }, error);
  }
};

const conditionalOperator = {
  1: 'Equal to',
  2: 'Greater than',
  3: 'Greater than or Equal to',
  4: 'Less than',
  5: 'Less than or Equal to',
  6: 'Starts with',
  7: 'Ends with',
  8: 'Contains',
};
